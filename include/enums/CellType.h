#pragma once

/// Loại ô trong 1 màn chơi, dùng bởi Level (lưu trữ) và LevelEditorState (chỉnh sửa).
/// Các giá trị SNAKE_LINK_*: KHÔNG phải hướng di chuyển của rắn (xem Direction.h),
/// mà là hướng NỐI TỚI đốt thân kế tiếp — dùng khi dựng lại rắn từ file đã lưu.
enum class CellType {
    EMPTY = 0,
    WALL,
    SNAKE_HEAD,
    SNAKE_LINK_UP,
    SNAKE_LINK_DOWN,
    SNAKE_LINK_LEFT,
    SNAKE_LINK_RIGHT
    // Sau này thêm chướng ngại mới (SPIKE, ITEM...) chỉ cần thêm dòng ở đây.
};