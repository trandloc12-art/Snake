# Luồng dữ liệu

Dự án là game local: “request” = input (bàn phím/chuột) mỗi frame. Không có HTTP, session server, hay DB. Sơ đồ dưới map tạm sang layer quen thuộc:

| Layer web điển hình | Tương đương trong Snake |
|---------------------|-------------------------|
| Controller | `*State::Update()` (đọc input, quyết định chuyển scene) |
| Service | `Snake`, `PlayingState` (ăn mồi, va chạm, spawn) |
| Repository | `Level::LoadFromFile` / `SaveToFile`, `LoadDirectoryFilesEx` |
| View | `*State::Draw()`, `SnakeRenderer` |
| Shared store | Member của `Game` |

---

## 1. Vòng lặp frame (luồng tổng)

```mermaid
flowchart TD
    A[main: Game.Run] --> B{WindowShouldClose?}
    B -->|không| C[Game.Update]
    B -->|có| Z[Game dtor: CloseWindow]

    C --> D{currentState != previousState?}
    D -->|có| E[State.Init]
    D -->|không| F[State.Update]
    E --> F

    F --> G[BeginDrawing + ClearBackground]
    G --> H[Game.Draw theo currentState]
    H --> I[EndDrawing]
    I --> B
```

`Game::ChangeState` chỉ gán `currentState`. `previousState` được cập nhật **cuối** `Update()`, nên `Init()` chạy đúng một lần khi vừa vào state (trừ lần vào MENU đầu tiên: `currentState == previousState == MENU` → `MenuState::Init()` **không** chạy lúc launch; giá trị mặc định vẫn đúng).

`quitRequested` được set từ menu nhưng **không** được `Run()` đọc — đóng cửa sổ vẫn chỉ qua nút X / `WindowShouldClose`.

---

## 2. Nơi lưu và truyền state

```mermaid
flowchart LR
    subgraph Game["Game — shared bag"]
        CS[currentState / previousState]
        PATH[selectedLevelPath]
        SCORE[lastScore]
        QUIT[quitRequested]
        AM[AssetManager.textures]
    end

    subgraph Disk["Đĩa"]
        LVL["assets/levels/*.txt"]
        TEX["assets/textures/*.png — cwd tương đối"]
    end

    LS[LevelSelectState] -->|SetSelectedLevelPath| PATH
    PATH -->|GetSelectedLevelPath| PS[PlayingState]
    PS -->|SetLastScore| SCORE
    SCORE -->|GetLastScore| GO[GameOverState]
    AM -->|const ref| PSR[SnakeRenderer trong PlayingState]
    LS -->|LoadDirectoryFilesEx| LVL
    PS -->|Level.LoadFromFile| LVL
    ED[LevelEditorState] -->|Level.SaveToFile| LVL
    AM -->|LoadTexture lúc start| TEX
```

### Bảng state

| Dữ liệu | Nơi sống | Ai ghi | Ai đọc | Thời gian sống |
|---------|----------|--------|--------|----------------|
| `currentState` | `Game` | Mọi state qua `ChangeState` | `Game::Update/Draw` | Cả phiên |
| `selectedLevelPath` | `Game` | `LevelSelectState` (full path). Default ctor: `"level1.txt"` (không có thư mục, file này không có trong repo) | `PlayingState::Init` | Cho đến khi chọn màn khác |
| `lastScore` | `Game` | `PlayingState` trước Game Over | `GameOverState::Draw` | Cho đến ván sau ghi đè |
| `quitRequested` | `Game` | `MenuState` | **Không ai** | — |
| Texture GPU | `AssetManager` trong `Game` | `LoadAll()` một lần | `SnakeRenderer` | Đến khi destroy `Game` |
| Lưới level | `Level` trong Playing hoặc Editor | Load file / SetCell / Init | Draw, Snake::InitFromLevel, va chạm, spawn mồi | Theo state object (không reset khi rời scene trừ lần `Init` kế) |
| Đốt rắn | `Snake::segments` | Init / Move | Renderer, OccupiesCell, collision | Trong `PlayingState` |
| `pendingDirection` | `PlayingState` | Phím mũi tên mỗi frame | `SetDirection` lúc step | Một nhịp di chuyển |
| `foodPosition`, `score`, `moveTimer` | `PlayingState` | SpawnFood / ăn / GetFrameTime | Update/Draw | Một ván |
| `levelFiles`, `selectedIndex` | `LevelSelectState` | `Init()` quét thư mục | Update/Draw | Mỗi lần vào select |
| Editor: `phase`, `inputBuffer`, `currentBrush`, `hasHead` | `LevelEditorState` | Input | Update/Draw | Phiên editor |

Không có: Redux/Context, cookie, session HTTP, thread khác, queue mạng.

Input raylib (`IsKeyPressed`, `GetMousePosition`) là **trạng thái frame hiện tại**, không lưu.

---

## 3. Luồng nghiệp vụ

Không có auth/payment. Các luồng quan trọng:

### 3.1 Menu → chọn hành động

```mermaid
sequenceDiagram
    actor User
    participant Menu as MenuState
    participant Game

    User->>Menu: UP/DOWN
    Menu->>Menu: selectedOption 0..2
    User->>Menu: ENTER
    alt Chơi
        Menu->>Game: ChangeState(LEVEL_SELECT)
    else Editor
        Menu->>Game: ChangeState(LEVEL_EDITOR)
    else Thoát hoặc ESC
        Menu->>Game: RequestQuit()
        Note over Game: Run() không kiểm tra cờ này
    end
```

### 3.2 Chọn màn → chơi

```mermaid
sequenceDiagram
    actor User
    participant Sel as LevelSelectState
    participant Game
    participant Play as PlayingState
    participant Level
    participant Disk as File .txt
    participant Snake

    Sel->>Sel: Init: LoadDirectoryFilesEx(LEVELS_DIRECTORY, .txt)
    Disk-->>Sel: danh sách path
    User->>Sel: UP/DOWN + ENTER
    Sel->>Game: SetSelectedLevelPath(fullPath)
    Sel->>Game: ChangeState(PLAYING)
    Game->>Play: Init()
    Play->>Level: LoadFromFile(path)
    Level->>Disk: ifstream từng token
    Disk-->>Level: grid CellType
    Play->>Snake: InitFromLevel(level)
    Note over Snake: Tìm HEAD, lần theo LINK_* trỏ ngược
    Play->>Play: SpawnFood trên ô EMPTY không trùng rắn
```

Đường levels: `PROJECT_SOURCE_DIR "assets/levels"` (source tree), **không** bản copy trong `build/`. Texture vẫn đọc `assets/textures/` theo **cwd** (thường là cạnh exe sau POST_BUILD copy).

### 3.3 Một bước gameplay (input → logic → render)

```mermaid
flowchart TD
    IN[Phím: hướng / ESC] --> PEND[pendingDirection]
    ESC[ESC] --> MENU[ChangeState MENU]
    T[moveTimer += GetFrameTime] --> CHK{timer >= 0.15s?}
    CHK -->|không| DRAW[Draw: tường, mồi, SnakeRenderer, điểm]
    CHK -->|có| RESET[moveTimer = 0]
    RESET --> SET[Snake.SetDirection pending — chặn 180°]
    SET --> PRED[Tính ô kế nextX/nextY]
    PRED --> FOOD{ô kế == foodPosition?}
    FOOD --> MOV[Snake.Move grow]
    MOV --> WALL{GetCell == WALL?}
    WALL -->|có| GO[SetLastScore + GAME_OVER]
    WALL -->|không| BODY{đầu trùng đốt i>=1?}
    BODY -->|có| GO
    BODY -->|không| ATE{ateFood?}
    ATE -->|có| SC[score++ / SpawnFood]
    ATE -->|không| DRAW
    SC --> DRAW
```

- Mồi: `GetRandomValue` trên danh sách ô trống. Hết ô → `SpawnFood` return, `foodPosition` có thể giữ giá trị cũ hoặc `-1,-1`.
- Tường ngoài lưới: `Level::GetCell` trả `WALL` → chết khi đi ra ngoài.
- Ô `HEAD`/`LINK_*` còn trên lưới **không** phải tường; rắn đi xuyên “dấu vết file”. Mồi không spawn trên các ô đó vì không `EMPTY`.

### 3.4 Thua → Game Over → Menu

```mermaid
flowchart LR
    P[PlayingState va chạm] --> S[Game.SetLastScore]
    S --> G[ChangeState GAME_OVER]
    G --> D[GameOverState.Draw đọc GetLastScore]
    D --> E[ENTER hoặc ESC]
    E --> M[ChangeState MENU]
```

Điểm **không** ghi ra đĩa (`note` đã ghi thiếu lưu điểm).

### 3.5 Level Editor → file

```mermaid
stateDiagram-v2
    [*] --> ASK_WIDTH: Init
    ASK_WIDTH --> ASK_HEIGHT: ENTER số
    ASK_HEIGHT --> DRAWING: ENTER số, Level.Init
    DRAWING --> ASK_FILENAME: phím S
    ASK_FILENAME --> DRAWING: ENTER lưu hoặc ý định ESC hủy
    DRAWING --> MENU: ESC
    ASK_WIDTH --> MENU: ESC
    ASK_HEIGHT --> MENU: ESC
```

Lưu:

1. `fullPath = LEVELS_DIRECTORY + "/" + tên + ".txt"`
2. `Level::SaveToFile`: tạo thư mục cha nếu thiếu → ghi token (`0`, `WALL`, `HEAD`, `LINK_*`)
3. `lastSaveMessage` chỉ trên UI editor

Chuột trái: `SetCell` theo brush. Chuột phải: xóa EMPTY. Brush HEAD bị chặn nếu `hasHead`.

Level Select lần vào sau sẽ quét lại thư mục → thấy file mới **không** cần rebuild (đúng ý comment trong `Constants.h`).

### 3.6 Asset load (khởi động)

```mermaid
flowchart TD
    W[InitWindow] --> L[AssetManager.LoadAll]
    L --> T1[snake_head / body / tail / wall / food]
    T1 --> MAP[unordered_map name → Texture2D]
    MAP --> R[SnakeRenderer.GetTexture khi Draw]
```

Load fail (`id == 0`): log stderr, vẫn insert map → vẽ texture rỗng. Không abort.

---

## 4. Format file level (hợp đồng dữ liệu)

- Mỗi dòng = một hàng; token phân tách khoảng trắng.
- `StringToCell` token lạ → `EMPTY` (nuốt lỗi).
- `width` = độ dài **hàng đầu**; hàng ngắn hơn vẫn được đẩy vào `grid` (rủi ro OOB — xem `BUGS_AND_ISSUES.md`).
- Rắn lúc chơi **không** cập nhật lại file; file chỉ là spawn layout.

Ví dụ repo (`assets/levels/test.txt`): khung `WALL`, một `HEAD`, không `LINK_*` → rắn 1 đốt, hướng mặc định `RIGHT` nếu không suy ra từ đốt 2.
