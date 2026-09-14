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

    /// Lưu lại trạng thái trước khi Move() để có thể Undo (quay lại bước trước).
    const std::deque<Vector2>& GetPrevSegments() const { return prevSegments; }

    /// Thời điểm (GetTime()) mỗi đốt được sinh ra, cùng thứ tự và cùng kích thước
    /// với GetSegments() (segmentSpawnTimes[i] tương ứng segments[i]).
    /// Dùng bởi SnakeRenderer để tính frame animation ổn định qua các bước di
    /// chuyển — vì index của 1 đốt vật lý trong "segments" bị dịch lên mỗi khi
    /// rắn đi (push_front đầu mới), animation không thể gắn theo index, phải
    /// gắn theo thời điểm sinh ra của chính đốt đó.
    const std::deque<double>& GetSegmentSpawnTimes() const { return segmentSpawnTimes; }

    /// Hướng di chuyển hiện tại — dùng bởi SnakeRenderer để xoay đầu rắn
    /// đúng hướng khi thân rắn chỉ có 1 đốt duy nhất (không có đốt kế để tính hướng).
    Direction GetCurrentDirection() const { return currentDirection; }

private:
    std::deque<Vector2> segments; // segments.front() = đầu, segments.back() = đuôi
    std::deque<Vector2> prevSegments; // vị trí trước lần Move() gần nhất, dùng để nội suy khi vẽ
    std::deque<double>  segmentSpawnTimes; // song song 1-1 với segments — xem GetSegmentSpawnTimes()
    Direction currentDirection = Direction::RIGHT;
};