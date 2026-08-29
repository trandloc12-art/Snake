#pragma once

/// Hướng di chuyển của rắn khi chơi. Không liên quan tới cách lưu trữ trong Level
/// (xem enums/CellType.h — dùng tên khác, SNAKE_LINK_*, để tránh nhầm 2 khái niệm).
enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};