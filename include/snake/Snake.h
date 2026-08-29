#pragma once
#include <deque>
#include "raylib.h"
#include "enums/Direction.h"

class Level; // forward declaration — chỉ cần con trỏ/tham chiếu trong InitFromLevel(),
             // không cần include Level.h đầy đủ ở đây, giảm phụ thuộc giữa các header.

/// Dữ liệu và logic thuần của rắn: vị trí, di chuyển, ăn mồi, tự cắn đuôi.
/// KHÔNG chứa bất kỳ lệnh vẽ nào — việc vẽ do SnakeRenderer đảm nhiệm.
class Snake {
public:
    /// Khởi tạo rắn thủ công tại 1 điểm, hướng, độ dài cho trước.
    void Init(int startX, int startY, Direction direction, int initialLength);

    /// Khởi tạo rắn bằng cách quét dữ liệu Level: tìm SNAKE_HEAD rồi lần theo
    /// các ô SNAKE_LINK_* để dựng đúng thứ tự thân rắn.
    void InitFromLevel(const Level& level);

    /// Đổi hướng di chuyển — chặn đổi ngược 180 độ.
    void SetDirection(Direction newDirection);

    /// Di chuyển 1 bước theo hướng hiện tại. grow=true -> không xóa đuôi -> dài ra.
    void Move(bool grow);

    Vector2 GetHeadPosition() const;
    bool OccupiesCell(int x, int y) const;
    const std::deque<Vector2>& GetSegments() const { return segments; }

private:
    std::deque<Vector2> segments; // segments.front() = đầu, segments.back() = đuôi
    Direction currentDirection = Direction::RIGHT;
};