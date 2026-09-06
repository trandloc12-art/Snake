# Bug và vấn đề tiềm ẩn

Phạm vi: logic, race/input, thiếu lỗi, leak, an toàn, trùng lặp. Ưu tiên các lỗ liên quan luồng ở `DATA_FLOW.md` (chuyển state, load/save level, bước rắn, tài nguyên GPU).

Không sửa code trong bước này — chỉ báo cáo.

---

## Critical

### 1. `PlayingState::Init` thất bại rồi vẫn `Update()` cùng frame → UB

**File:** `src/Game.cpp` khoảng 58–60; `src/states/PlayingState.cpp` 13–32, 76–84; `src/snake/Snake.cpp` 145–147, 130–142.

Khi load file lỗi hoặc không có `SNAKE_HEAD`, `Init()` gọi `ChangeState(MENU)` rồi **return**. `Game::Update` vẫn gọi `playingState.Update()` ngay sau đó vì `ChangeState` không dừng nhánh hiện tại.

`Update()` gọi `snake.GetHeadPosition()` → `segments.front()` trên `deque` rỗng = **undefined behavior** (crash). `Move` trên rắn rỗng cũng UB.

**Hướng fix:** Sau `Init()`, nếu `currentState` đã đổi thì `break` / `return`. Hoặc `Init` trả `bool`; hoặc không gọi `Update` khi `Init` fail. `GetHeadPosition`/`Move` nên no-op hoặc assert khi rỗng.

---

### 2. `Snake::InitFromLevel` vòng lặp vô hạn / phình bộ nhớ

**File:** `src/snake/Snake.cpp` 70–105.

Thuật toán “lần theo link trỏ ngược” **không đánh dấu ô đã thăm**. Hai ô `LINK` kề nhau trỏ vào nhau (dễ vẽ trong Editor: `LINK_RIGHT` cạnh `LINK_LEFT`) → `while (true)` thêm segment mãi, treo hoặc OOM.

Luồng: chọn màn chứa layout lỗi → `PlayingState::Init` → không bao giờ tới `SpawnFood`.

**Hướng fix:** `unordered_set` ô đã thăm; break nếu visit lại. Giới hạn `segments.size() <= width*height`. Validate editor trước save (một HEAD, chain đơn, không cycle).

---

### 3. `RequestQuit` không thoát game

**File:** `include/Game.h` 34, 54; `src/Game.cpp` 31–39; `src/states/MenuState.cpp` 14–16, 26.

Menu ESC / mục “Thoát” set `quitRequested = true`. `Run()` chỉ `while (!WindowShouldClose())` — **không đọc cờ**. Người chơi không thoát được bằng menu (trừ nút đóng cửa sổ).

**Hướng fix:** `while (!WindowShouldClose() && !quitRequested)` hoặc `CloseWindow` có kiểm soát. Có thể `SetWindowShouldClose` nếu raylib version hỗ trợ.

---

## Major

### 4. Hủy nhập tên file (ESC) trong Editor chết — luôn về Menu

**File:** `src/states/LevelEditorState.cpp` 18–22 vs 78–82.

Đầu `Update()`: ESC → `ChangeState(MENU)` + return. Khối `ASK_FILENAME` xử lý ESC “hủy, về DRAWING” **không bao giờ chạy**. UI vẫn ghi `"ENTER: Luu | ESC: Huy"` (`Draw` dòng 161).

**Hướng fix:** Xử lý ESC theo `phase`: `ASK_FILENAME` → DRAWING; phase khác → Menu.

---

### 5. `Level::LoadFromFile` hàng răng cưa → out-of-bounds

**File:** `src/snake/Level.cpp` 47–65, 101–103.

`width` lấy từ `grid[0].size()`. Hàng sau ngắn hơn vẫn `push_back`. `GetCell(x,y)` chỉ check `x >= width` (width hàng đầu), **không** check `x >= grid[y].size()` → `grid[y][x]` OOB.

Token lạ → `EMPTY` im lặng (`StringToCell` 37–38) — level “load thành công” nhưng sai.

`PlayingState::Init` chỉ check `is_open` gián tiếp qua `LoadFromFile == true` (luôn `true` nếu mở được file, kể cả file rỗng). File rỗng: `height=0`, `InitFromLevel` không thấy HEAD → dính bug #1.

**Hướng fix:** Từ chối hàng lệch độ dài; `LoadFromFile` false nếu rỗng/lệch. `GetCell` bound theo từng hàng. Log token unknown.

---

### 6. Unload texture sau `CloseWindow` (GPU context đã chết)

**File:** `src/Game.cpp` 20–22; `include/Game.h` 43 vs 48–52; `src/core/AssetManager.cpp` 6–8, 57–61.

`~Game()` gọi `CloseWindow()` **trước** khi member bị hủy. `assets` khai báo trước các state → hủy **sau** body destructor → `UnloadAll()` / `UnloadTexture` sau khi context OpenGL đã đóng. Rủi ro leak driver hoặc crash lúc thoát.

**Hướng fix:** `UnloadAll()` (hoặc destroy `AssetManager` tường minh) **trước** `CloseWindow()`. Đừng dựa vào thứ tự member sau khi đã close window.

---

### 7. Buffer hướng 1 ô: mất input “rẽ rồi rẽ”

**File:** `src/states/PlayingState.cpp` 66–75; `src/snake/Snake.cpp` 118–127.

Chỉ một `pendingDirection`. Đang `RIGHT`, trong một `moveInterval` bấm `UP` rồi `LEFT`: pending = `LEFT` → `SetDirection` coi là 180° so với `RIGHT` → **bỏ cả hai**. Rắn không rẽ.

**Hướng fix:** Queue tối đa 2 hướng; hoặc áp dụng hướng ngay khi không ngược với hướng **đã scheduled**, không chỉ hướng đang chạy.

---

### 8. Hằng MIN/MAX size editor không dùng — lưới 0 hoặc 999×999

**File:** `include/core/Constants.h` 22–25; `src/states/LevelEditorState.cpp` 35–44.

`std::stoi` chấp nhận 1–999. `Level::Init(0, n)` hoặc `999×999` không clamp. Lưới lớn: vẽ O(w×h) mỗi frame không camera (`WINDOW` 800×600, `cellSize` 32) — phần map ngoài màn, hit-test chuột theo pixel cửa sổ không pan → không sửa được ô ngoài. `stoi` không try/catch (buffer chỉ digit nên ít throw).

**Hướng fix:** Clamp `[MIN_LEVEL_*, MAX_LEVEL_*]` trước `Init`. Camera/zoom hoặc cấm size > viewport. Báo lỗi input.

---

### 9. `hasHead` lệch với lưới

**File:** `src/states/LevelEditorState.cpp` 99–109.

- Đè HEAD bằng brush khác (WALL/EMPTY/LINK) không clear `hasHead` → **không đặt HEAD mới**.
- Vẽ HEAD lên ô HEAD: `hasHead` true nên nhánh bỏ qua — OK.
- Không quét lại lưới khi load (editor không load file có sẵn).

**Hướng fix:** Khi `SetCell` đè ô HEAD → `hasHead = false`. Hoặc `hasHead = (đếm HEAD == 1)` sau mỗi edit. Cho phép di chuyển HEAD (xóa cái cũ).

---

### 10. Cửa sổ / map / tile không khớp (ghi trong `note`)

**File:** `include/core/Constants.h` 8–15; `PlayingState`/`LevelEditorState` `cellSize = 32`; `AssetManager` cảnh báo size texture.

Map lớn hơn ~25×18 ô bị cắt. Texture thiếu trong repo + cwd sai → `id==0` (xem #14). Không scale theo `GetScreenWidth`.

**Hướng fix:** Camera follow đầu rắn; `cellSize = min(W/width, H/height)`; đóng gói texture; path texture dựa `GetApplicationDirectory()` giống ý levels.

---

### 11. Path levels vs texture không cùng gốc

**File:** `include/core/Constants.h` 32–43; `src/main.cpp` 6; `CMakeLists.txt` 24–35, 30–35.

Levels: path compile-time `PROJECT_SOURCE_DIR`. Texture: `"assets/textures/"` theo **working directory**. Chạy exe không từ `build/` (hoặc shortcut cwd khác) → texture fail, levels vẫn thấy. `main` in cwd (debug để lại).

File mặc định `DEFAULT_LEVEL_FILE = "level1.txt"` không tồn tại; không gắn `LEVELS_DIRECTORY`. Chỉ an toàn vì chơi đi qua Level Select (full path).

**Hướng fix:** Một root (`GetApplicationDirectory()` hoặc luôn source/assets). Bỏ `level1.txt` hoặc thêm file + path đủ.

---

## Minor

### 12. `MenuState::Init` không chạy lần đầu

**File:** `src/Game.cpp` 4–6, 43; `src/states/MenuState.cpp` 8–10.

`currentState == previousState == MENU` lúc construct → `justEntered` false. Hiện `selectedOption` default 0 nên UI vẫn đúng.

**Hướng fix:** `previousState` sentinel / `bool firstFrame` / gọi `menuState.Init()` trong constructor `Game`.

---

### 13. `GameOverState::Update` xử lý ENTER hai lần

**File:** `src/states/GameOverState.cpp` 11–17.

`ESC || ENTER` đã `ChangeState(MENU)`; block `ENTER` lặp lại. Không vỡ vì `ChangeState` idempotent.

**Hướng fix:** Xóa block thứ hai.

---

### 14. Load texture thất bại vẫn tiếp tục

**File:** `src/core/AssetManager.cpp` 23–28, 46–54.

`id==0` chỉ `cerr`. `GetTexture` miss → `assert` (mất ở Release) + `static Texture2D empty`. Draw vẫn gọi.

**Hướng fix:** Fail fast lúc `LoadAll` hoặc placeholder 64×64. Không `assert` rồi trả dummy im lặng ở Release.

---

### 15. `SpawnFood` hết chỗ / `foodPosition` (-1,-1)

**File:** `src/states/PlayingState.cpp` 36–37, 54, 82.

Hết ô trống: return, không set “thắng”. Mồi cũ vẫn vẽ (có thể nằm dưới rắn). Nếu chưa spawn lần nào và so sánh với `(-1,-1)` khi đầu ra ngoài map: `nextX==-1` có thể `ateFood==true` trước khi chết tường — edge case.

Ô `HEAD`/`LINK_*` trên file không spawn mồi và không chặn rắn.

**Hướng fix:** Hết ô → state thắng. Clear mồi. Spawn coi “passable” = không WALL không rắn. Tùy chọn xóa HEAD/LINK khỏi lưới sau `InitFromLevel`.

---

### 16. Va chạm kiểm tra **sau** `Move`

**File:** `src/states/PlayingState.cpp` 84–102.

Rắn đã vào tường một frame rồi mới Game Over (vẽ một frame đâm tường). Self-hit tương tự.

**Hướng fix:** Predict ô kế (đã có `nextX/Y`) — nếu tường/thân thì không `Move`, vào Game Over.

---

### 17. `moveTimer = 0` thay vì trừ interval

**File:** `src/states/PlayingState.cpp` 71–73.

Frame lag > interval chỉ đi **một** bước (có thể cố ý). Timer không bù phần dư → tốc độ hơi lệch.

**Hướng fix:** `while (moveTimer >= interval) { moveTimer -= interval; step; }` hoặc clamp.

---

### 18. Include / trùng lặp / case path

| Vấn đề | File | Mức | Gợi ý |
|--------|------|-----|--------|
| `#include "snake/Level.h"` hai lần | `src/snake/Level.cpp` 2–3 | Minor | Xóa một dòng |
| `"Snake/Snake.h"` vs thư mục `include/snake` | `SnakeRenderer.h`, `Snake.cpp` | Major trên Linux (case-sensitive) | Thống nhất `snake/...` |
| Wall/food texture load nhưng Playing vẽ `DrawRectangle` | `AssetManager.cpp` 16–17; `PlayingState.cpp` 112–123 | Minor | Dùng texture hoặc đừng load |
| `cellSize` hardcode 32, đã có `DEFAULT_CELL_SIZE` | Playing/Editor | Minor | Dùng hằng |
| List màn không scroll | `LevelSelectState.cpp` 63–66 | Minor | Clamp index visible / cuộn |
| Log debug `cout` | `main.cpp`, `LevelSelectState::Init` | Minor | `#ifdef DEBUG` |
| `Init()` thủ công `Snake::Init` không dùng | `Snake.cpp` 29–44 | Minor | Dead code hoặc fallback khi không có HEAD |
| Editor LINK cùng màu, không phân biệt hướng | `LevelEditorState.cpp` 143–146 | Minor | Mũi tên / màu khác |
| Không audio / không lưu high score | `note` | Feature | Ngoài bug runtime |

---

### 19. `AssetManager::LoadAll` gọi lần 2 sẽ leak GPU

**File:** `src/core/AssetManager.cpp` 10–18, 57–61.

Ghi đè map không `UnloadTexture` cũ. Hiện chỉ gọi một lần — rủi ro nếu sau này reload.

**Hướng fix:** `UnloadAll()` đầu `LoadAll`.

---

### 20. An toàn (phạm vi desktop local)

Không có mạng/auth. Tên file editor lọc `[A-Za-z0-9_]`, path = `LEVELS_DIRECTORY + name + .txt` — không traversal. `LEVELS_DIRECTORY` là path build-time tuyệt đối: ghi được vào source tree (đúng design, không phải sandbox).

`LoadFromFile` không giới hạn kích thước file → file độc hại hàng triệu token có thể làm phình RAM (Major nếu coi level là untrusted).

---

## Bản đồ theo luồng DATA_FLOW

| Luồng | Issue IDs |
|-------|-----------|
| Vòng lặp / quit / chuyển state | #1, #3, #12, #13 |
| Chọn màn + load file | #5, #11, #2 |
| Bước chơi + mồi + chết | #1, #7, #15, #16, #17 |
| Editor + save | #4, #8, #9, #18 UI |
| Asset + thoát process | #6, #10, #14, #19 |

---

## Ghi chú từ file `note` (chưa làm)

- Scale ảnh / cửa sổ, intro, audio, high score, co-op.
- “chưa load được màn chơi” — khớp #11 (cwd/path), thiếu `level1.txt`, và/hoặc #1 khi load fail.
