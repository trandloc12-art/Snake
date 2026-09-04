#pragma once

// ============================================================
// Các hằng số dùng chung cho TOÀN BỘ dự án
// ============================================================

// ---------- Cửa sổ game ----------
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr int TARGET_FPS = 60;
constexpr const char* WINDOW_TITLE = "Snake Game";

// ---------- Sprite / Rendering ----------
constexpr int TILE_SOURCE_SIZE = 64;
constexpr int DEFAULT_CELL_SIZE = 32;

// ---------- Gameplay: Rắn ----------
constexpr int DEFAULT_SNAKE_LENGTH = 3;
constexpr float DEFAULT_MOVE_INTERVAL = 0.15f;

// ---------- Gameplay: Level Editor ----------
constexpr int MAX_LEVEL_WIDTH = 100;
constexpr int MAX_LEVEL_HEIGHT = 100;
constexpr int MIN_LEVEL_WIDTH = 5;
constexpr int MIN_LEVEL_HEIGHT = 5;

// ---------- Đường dẫn thư mục ----------
// QUAN TRỌNG: level do Editor tạo cần đọc/ghi TRỰC TIẾP vào thư mục gốc project
// (không phải build/), để: (1) không bị mất khi xóa build/, (2) commit được lên Git,
// (3) LevelSelectState thấy ngay file mới mà không cần build lại.
// PROJECT_SOURCE_DIR được CMake nhúng lúc biên dịch (xem target_compile_definitions).
#ifdef PROJECT_SOURCE_DIR
    constexpr const char* LEVELS_DIRECTORY = PROJECT_SOURCE_DIR "assets/levels";
#else
    // Dự phòng nếu ai đó build không qua CMake (hiếm khi xảy ra) - dùng path tương đối cũ.
    constexpr const char* LEVELS_DIRECTORY = "assets/levels";
#endif

// Texture thì KHÔNG cần đổi - vẫn đọc từ build/assets/textures/ vì ảnh không đổi lúc chạy,
// việc CMake copy sẵn vào build/ là đủ, không cần đọc trực tiếp từ source.
constexpr const char* TEXTURES_DIRECTORY = "assets/textures/";

constexpr const char* DEFAULT_LEVEL_FILE = "level1.txt"; // chỉ tên file, LEVELS_DIRECTORY đã có path đầy đủ