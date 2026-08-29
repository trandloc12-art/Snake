#include "Snake/Snake.h"
#include "Snake/Level.h"
#include "enums/CellType.h"

// Hàm helper CHỈ dùng nội bộ file này (anonymous namespace = giới hạn phạm vi
// trong file .cpp, tương tự "private" nhưng ở cấp độ file thay vì class).
namespace {
    Direction CellLinkToDirection(CellType cell) {
        switch (cell) {
            case CellType::SNAKE_LINK_UP:    return Direction::UP;
            case CellType::SNAKE_LINK_DOWN:  return Direction::DOWN;
            case CellType::SNAKE_LINK_LEFT:  return Direction::LEFT;
            case CellType::SNAKE_LINK_RIGHT: return Direction::RIGHT;
            default:                         return Direction::RIGHT; // không nên xảy ra
        }
    }

    // Trả về ô liền kề theo 1 hướng — dùng để "lần theo dây" từ đầu tới đuôi.
    void StepInDirection(int& x, int& y, Direction dir) {
        switch (dir) {
            case Direction::UP:    y -= 1; break;
            case Direction::DOWN:  y += 1; break;
            case Direction::LEFT:  x -= 1; break;
            case Direction::RIGHT: x += 1; break;
        }
    }
}

void Snake::Init(int startX, int startY, Direction direction, int initialLength) {
    segments.clear();
    currentDirection = direction;

    for (int i = 0; i < initialLength; i++) {
        int x = startX;
        int y = startY;
        switch (direction) {
            case Direction::RIGHT: x -= i; break;
            case Direction::LEFT:  x += i; break;
            case Direction::DOWN:  y -= i; break;
            case Direction::UP:    y += i; break;
        }
        segments.push_back({ (float)x, (float)y });
    }
}

void Snake::InitFromLevel(const Level& level) {
    segments.clear();

    // Bước 1: quét toàn bộ lưới để tìm ô SNAKE_HEAD.
    int headX = -1, headY = -1;
    for (int y = 0; y < level.GetHeight(); y++) {
        for (int x = 0; x < level.GetWidth(); x++) {
            if (level.GetCell(x, y) == CellType::SNAKE_HEAD) {
                headX = x;
                headY = y;
                break;
            }
        }
        if (headX != -1) break;
    }

    if (headX == -1) return; // không tìm thấy HEAD -> level lỗi, không dựng được rắn

    segments.push_back({ (float)headX, (float)headY });

    // Bước 2: lần theo dây từ đầu — mỗi ô SNAKE_LINK_* cho biết đốt kế tiếp nằm ở đâu.
    int currentX = headX;
    int currentY = headY;

    while (true) {
        CellType cell = level.GetCell(currentX, currentY);
        bool isLink = (cell == CellType::SNAKE_LINK_UP || cell == CellType::SNAKE_LINK_DOWN ||
                       cell == CellType::SNAKE_LINK_LEFT || cell == CellType::SNAKE_LINK_RIGHT);

        // Ô đầu (SNAKE_HEAD) không tự chứa hướng nối - cần đọc hướng từ ô kế cận
        // theo quy ước: ô ngay sau HEAD trong file phải là 1 ô SNAKE_LINK_*.
        // Ở đây ta tìm ô liền kề (4 hướng) có SNAKE_LINK_* trỏ NGƯỢC vào ô hiện tại.
        Direction foundDir;
        bool found = false;

        for (Direction dir : { Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT }) {
            int nx = currentX, ny = currentY;
            StepInDirection(nx, ny, dir);
            CellType neighbor = level.GetCell(nx, ny);

            bool isNeighborLink = (neighbor == CellType::SNAKE_LINK_UP || neighbor == CellType::SNAKE_LINK_DOWN ||
                                    neighbor == CellType::SNAKE_LINK_LEFT || neighbor == CellType::SNAKE_LINK_RIGHT);
            if (!isNeighborLink) continue;

            // Ô láng giềng phải "trỏ ngược" về currentX/currentY thì mới đúng là đốt kế tiếp
            Direction linkDir = CellLinkToDirection(neighbor);
            int backX = nx, backY = ny;
            StepInDirection(backX, backY, linkDir);
            if (backX == currentX && backY == currentY) {
                foundDir = dir;
                found = true;
                break;
            }
        }

        if (!found) break; // đã tới đuôi, không còn đốt tiếp theo

        StepInDirection(currentX, currentY, foundDir);
        segments.push_back({ (float)currentX, (float)currentY });
    }

    // Hướng di chuyển ban đầu: suy ra từ đầu -> đốt thứ 2 (nếu có từ 2 đốt trở lên)
    if (segments.size() >= 2) {
        Vector2 head = segments[0];
        Vector2 next = segments[1];
        if (next.x < head.x) currentDirection = Direction::RIGHT;
        else if (next.x > head.x) currentDirection = Direction::LEFT;
        else if (next.y < head.y) currentDirection = Direction::DOWN;
        else currentDirection = Direction::UP;
    }
}

void Snake::SetDirection(Direction newDirection) {
    bool isOpposite =
        (currentDirection == Direction::UP && newDirection == Direction::DOWN) ||
        (currentDirection == Direction::DOWN && newDirection == Direction::UP) ||
        (currentDirection == Direction::LEFT && newDirection == Direction::RIGHT) ||
        (currentDirection == Direction::RIGHT && newDirection == Direction::LEFT);

    if (!isOpposite) {
        currentDirection = newDirection;
    }
}

void Snake::Move(bool grow) {
    Vector2 head = segments.front();
    switch (currentDirection) {
        case Direction::UP:    head.y -= 1; break;
        case Direction::DOWN:  head.y += 1; break;
        case Direction::LEFT:  head.x -= 1; break;
        case Direction::RIGHT: head.x += 1; break;
    }

    segments.push_front(head);
    if (!grow) {
        segments.pop_back();
    }
}

Vector2 Snake::GetHeadPosition() const {
    return segments.front();
}

bool Snake::OccupiesCell(int x, int y) const {
    for (const auto& seg : segments) {
        if ((int)seg.x == x && (int)seg.y == y) return true;
    }
    return false;
}