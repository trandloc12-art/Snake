#pragma once

// ============================================================
// Các hằng số dùng chung cho TOÀN BỘ dự án (không riêng module nào).
// Khi thêm game mới (TicTacToe...), hằng số nào chỉ riêng module đó
// thì nên khai báo trong file riêng của module, không thêm vào đây.
// ============================================================

// ---------- Cửa sổ game ----------
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr int TARGET_FPS = 60;
constexpr const char* WINDOW_TITLE = "Snake Game";

// ---------- Sprite / Rendering ----------
/// Kích thước gốc (pixel) của mọi sprite — TẤT CẢ texture phải vẽ đúng kích thước này.
constexpr int TILE_SOURCE_SIZE = 64;

/// Kích thước 1 ô khi hiển thị lên màn hình — PHẢI là bội số nguyên của TILE_SOURCE_SIZE
/// (16, 32, 48, 64...) để tránh mờ ảnh khi scale (xem SnakeRenderer::Draw()).
constexpr int DEFAULT_CELL_SIZE = 32;

// ---------- Gameplay: Rắn ----------
/// Số đốt thân ban đầu khi bắt đầu 1 màn (dùng khi Init() thủ công, không load từ Level).
constexpr int DEFAULT_SNAKE_LENGTH = 3;

/// Thời gian (giây) giữa mỗi lần rắn di chuyển 1 ô — càng nhỏ càng nhanh.
constexpr float DEFAULT_MOVE_INTERVAL = 0.15f;

// ---------- Gameplay: Level Editor ----------
/// Giới hạn kích thước lưới tối đa người dùng có thể nhập trong Editor,
/// tránh nhập số quá lớn gây lag hoặc tràn bộ nhớ.
constexpr int MAX_LEVEL_WIDTH = 100;
constexpr int MAX_LEVEL_HEIGHT = 100;

/// Kích thước lưới tối thiểu hợp lệ — dưới mức này màn chơi không đủ chỗ cho rắn di chuyển.
constexpr int MIN_LEVEL_WIDTH = 5;
constexpr int MIN_LEVEL_HEIGHT = 5;

// ---------- Đường dẫn thư mục (đường dẫn tương đối tính từ nơi chạy executable) ----------
constexpr const char* LEVELS_DIRECTORY = "assets/levels/";
constexpr const char* TEXTURES_DIRECTORY = "assets/textures/";

/// File màn chơi mặc định — dùng khi LevelEditorState lưu lần đầu, hoặc PlayingState
/// load nếu chưa có LevelSelectState chọn màn cụ thể.
constexpr const char* DEFAULT_LEVEL_FILE = "assets/levels/level1.txt";