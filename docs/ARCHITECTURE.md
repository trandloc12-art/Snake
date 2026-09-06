# Kiến trúc dự án Snake Game

Game Snake desktop (C++17), vòng lặp cố định 60 FPS, điều phối bằng **state machine**. Không có server, database, HTTP hay layer controller/service/repository. “Persistence” duy nhất là file text trong `assets/levels/`.

---

## 1. Cấu trúc thư mục chính

```
Snake/
├── CMakeLists.txt          # Build: C++17, link raylib, copy assets, nhúng PROJECT_SOURCE_DIR
├── README.md
├── note                    # Ghi chú nội bộ (ý tưởng / bug đã biết)
├── docs/                   # Tài liệu (thư mục này)
├── include/                # Header công khai theo module
│   ├── Game.h
│   ├── core/               # Hằng số + quản lý texture
│   ├── enums/              # GameState, Direction, CellType
│   ├── snake/              # Domain: bản đồ + rắn + renderer
│   └── states/             # Từng màn hình (Menu, Select, Editor, Play, GameOver)
├── src/                    # Implementation, song song với include/
│   ├── main.cpp
│   ├── Game.cpp
│   ├── core/
│   ├── snake/
│   └── states/
└── assets/
    └── levels/             # File màn chơi .txt (test.txt, text.txt)
```

`assets/textures/` được `AssetManager` kỳ vọng lúc chạy (`snake_head.png`, `wall.png`, …) nhưng **không có trong repo** — CMake chỉ copy `assets/` vào thư mục cạnh file exe nếu thư mục nguồn tồn tại.

Thư mục bị bỏ qua khi đọc codebase: `node_modules`, `build`, `dist` (không có trong project). `CMakeLists.txt` trỏ raylib tuyệt đối: `C:/libs/raylib-6.0`.

---

## 2. Vai trò từng module

| Module | Files | Vai trò |
|--------|--------|---------|
| **Entry** | `src/main.cpp` | Tạo `Game`, gọi `Run()`. In working directory ra stdout. |
| **Game (orchestrator)** | `include/Game.h`, `src/Game.cpp` | Sở hữu cửa sổ raylib, `AssetManager`, mọi state, dữ liệu chia sẻ (`selectedLevelPath`, `lastScore`, `quitRequested`). Vòng lặp Update/Draw + chuyển state. |
| **Constants** | `include/core/Constants.h` | Kích thước cửa sổ, FPS, tile, kích thước level, đường dẫn levels/textures. |
| **AssetManager** | `include/core/AssetManager.h`, `src/core/AssetManager.cpp` | Load/unload `Texture2D`, cấm copy, lookup theo tên. |
| **Enums** | `include/enums/*.h` | `GameState`, `Direction` (di chuyển), `CellType` (ô bản đồ / link thân rắn). |
| **Level** | `include/snake/Level.h`, `src/snake/Level.cpp` | Lưới `vector<vector<CellType>>`, load/save file token, Get/Set có clamp (ngoài lưới = WALL khi đọc). |
| **Snake** | `include/snake/Snake.h`, `src/snake/Snake.cpp` | Logic thuần: `deque<Vector2>` (đầu = `front()`), Init / InitFromLevel, đổi hướng (chặn 180°), Move, va chạm ô. **Không vẽ.** |
| **SnakeRenderer** | `include/snake/SnakeRenderer.h`, `src/snake/SnakeRenderer.cpp` | Chỉ vẽ rắn (head/body/tail/corner) từ `AssetManager`. |
| **MenuState** | `include/states/MenuState.h`, `src/states/MenuState.cpp` | Menu: Chơi / Editor / Thoát. |
| **LevelSelectState** | `include/states/LevelSelectState.h`, `src/states/LevelSelectState.cpp` | Quét `LEVELS_DIRECTORY` (`.txt`), chọn màn, ghi path vào `Game`. |
| **LevelEditorState** | `include/states/LevelEditorState.h`, `src/states/LevelEditorState.cpp` | Wizard: width → height → vẽ chuột → nhập tên file → `Level::SaveToFile`. |
| **PlayingState** | `include/states/PlayingState.h`, `src/states/PlayingState.cpp` | Load level, dựng rắn, spawn mồi, input, step theo timer, ăn mồi, wall/self-hit → Game Over. |
| **GameOverState** | `include/states/GameOverState.h`, `src/states/GameOverState.cpp` | Hiện `lastScore`, Enter/Esc về Menu. |

### Quan hệ sở hữu (composition)

- `Game` **sở hữu duy nhất** `AssetManager` và tất cả state objects (không heap, không stack state riêng).
- State nhận `Game&` để `ChangeState` / đọc path / điểm.
- `PlayingState` nhận thêm `const AssetManager&` cho `SnakeRenderer`.
- `Level` / `Snake` sống **bên trong** `PlayingState` hoặc `LevelEditorState`, không phải singleton.

Thứ tự member trong `Game` cố ý: `assets` khai báo trước các state để constructor state có thể nhận tham chiếu assets (dù texture chỉ load trong body sau `InitWindow`).

---

## 3. Entry points

| Vai trò | Vị trí | Hành vi |
|---------|--------|---------|
| **main** | `src/main.cpp` | `Game game; game.Run();` |
| **Vòng lặp game** | `Game::Run()` trong `src/Game.cpp` | `while (!WindowShouldClose())` → `Update()` → `BeginDrawing` / `Draw()` / `EndDrawing`. |
| **Chuyển scene** | `Game::ChangeState` | Gán `currentState`. `Init()` của state mới chạy 1 lần khi `currentState != previousState`. |
| **Build / “server start”** | Không có server | `cmake` + `add_executable(SnakeGame)` từ `file(GLOB_RECURSE src/*.cpp)`. |

Khởi tạo cửa sổ: `Game::Game()` gọi `InitWindow(800, 600, "Snake Game")`, `SetExitKey(KEY_NULL)` (tắt ESC mặc định của raylib), `SetTargetFPS(60)`, `assets.LoadAll()`.

---

## 4. Công nghệ / framework

| Thành phần | Chi tiết |
|------------|----------|
| Ngôn ngữ | C++17 (`CMAKE_CXX_STANDARD 17`) |
| Build | CMake ≥ 3.16, `CMAKE_EXPORT_COMPILE_COMMANDS ON` |
| Rendering / input / window / RNG | [raylib](https://www.raylib.com/) 6.0 (path máy local `C:/libs/raylib-6.0`) |
| STL | `deque`, `vector`, `unordered_map`, `filesystem`, `fstream`, `string` |
| GPU assets | `Texture2D` + `DrawTexturePro` (rắn); tường/mồi đang vẽ `DrawRectangle` |
| Persistence | File `.txt` token (không SQL, không JSON API) |
| IDE phụ | `.vscode/c_cpp_properties.json`, `.vscode/settings.json` |

Không dùng: mạng, auth, payment, ORM, UI framework (ImGui, v.v.), audio (ghi chú trong `note`).

---

## 5. Pattern kiến trúc

```
main
  └─ Game                    # Facade + state machine + shared bag
       ├─ AssetManager       # Resource owner
       └─ *State             # Mỗi state: Init / Update / Draw
            ├─ Level         # Data + I/O
            ├─ Snake         # Domain logic
            └─ SnakeRenderer # View
```

- **State pattern** (enum + object sẵn, không virtual `IState`).
- **Tách data/render** cho rắn (`Snake` vs `SnakeRenderer`).
- **Shared state** trên `Game` thay vì context/store toàn cục.
- **Không** DI container, không event bus.
