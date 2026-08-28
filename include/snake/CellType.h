// include/snake/CellType.h
#pragma once

enum class CellType {
    EMPTY = 0,
    WALL,
    SNAKE_UP,     // ô rắn, đoạn tiếp theo (về đuôi) nằm phía trên
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT,
    SNAKE_HEAD    // đánh dấu ô nào là đầu — bắt buộc phải có 1 ô loại này
    // sau này thêm chướng ngại mới: SPIKE, ITEM... chỉ cần thêm dòng ở đây
};