// SnakeRenderer.cpp
#include "Snake/SnakeRenderer.h"
#include "core/Constants.h"
#include <cassert>

namespace {
    Direction DirectionFromTo(Vector2 from, Vector2 to) {
        if (to.x > from.x) return Direction::RIGHT;
        if (to.x < from.x) return Direction::LEFT;
        if (to.y > from.y) return Direction::DOWN;
        return Direction::UP;
    }

    // Đầu vẽ sẵn chỉ LÊN (UP) = 0°. Xoay CW: UP -> RIGHT -> DOWN -> LEFT = 0/90/180/270.
    // (dùng cho ĐẦU)
    float HeadRotation(Direction dir) {
        switch (dir) {
            case Direction::UP:    return 0.0f;
            case Direction::RIGHT: return 90.0f;
            case Direction::DOWN:  return 180.0f;
            case Direction::LEFT:  return 270.0f;
        }
        return 0.0f;
    }

    // Đuôi vẽ sẵn chỉ XUỐNG (DOWN) = 0°. Xoay CW: DOWN -> LEFT -> UP -> RIGHT = 0/90/180/270.
    // (dùng RIÊNG cho ĐUÔI — khác offset với đầu vì 2 ảnh vẽ ngược hướng nhau)
    float TailRotation(Direction dir) {
        switch (dir) {
            case Direction::DOWN:  return 0.0f;
            case Direction::LEFT:  return 90.0f;
            case Direction::UP:    return 180.0f;
            case Direction::RIGHT: return 270.0f;
        }
        return 0.0f;
    }

    bool IsStraight(Direction dirIn, Direction dirOut) {
        return (dirIn == Direction::UP && dirOut == Direction::DOWN) ||
               (dirIn == Direction::DOWN && dirOut == Direction::UP) ||
               (dirIn == Direction::LEFT && dirOut == Direction::RIGHT) ||
               (dirIn == Direction::RIGHT && dirOut == Direction::LEFT);
    }

    // Thân thẳng vẽ sẵn DỌC (nối UP-DOWN) = 0° — giữ nguyên, đúng với ảnh.
    float StraightRotation(Direction dirIn) {
        bool vertical = (dirIn == Direction::UP || dirIn == Direction::DOWN);
        return vertical ? 0.0f : 90.0f;
    }

    // Góc cua vẽ sẵn nối LEFT + DOWN = 0°.
    // Xoay 90° CW mỗi bước: LEFT+DOWN -> UP+LEFT -> RIGHT+UP -> DOWN+RIGHT.
    float CornerRotation(Direction dirIn, Direction dirOut) {
        bool hasLeft  = (dirIn == Direction::LEFT  || dirOut == Direction::LEFT);
        bool hasDown  = (dirIn == Direction::DOWN  || dirOut == Direction::DOWN);
        bool hasUp    = (dirIn == Direction::UP    || dirOut == Direction::UP);
        bool hasRight = (dirIn == Direction::RIGHT || dirOut == Direction::RIGHT);

        if (hasLeft  && hasDown)  return 0.0f;   // nối LEFT + DOWN
        if (hasUp    && hasLeft)  return 90.0f;  // nối UP + LEFT
        if (hasRight && hasUp)    return 180.0f; // nối RIGHT + UP
        return 270.0f;                            // nối DOWN + RIGHT
    }
}

SnakeRenderer::SnakeRenderer(const AssetManager& assets) : assets(assets) {}

void SnakeRenderer::Draw(const Snake& snake, int cellSize) const {
    const auto& segments = snake.GetSegments();
    size_t lastIndex = segments.size() - 1;

    for (size_t i = 0; i < segments.size(); i++) {
        const Texture2D* tex;
        float rotation = 0.0f;

        if (i == 0) {
            tex = &assets.GetTexture("snake_head");
            if (segments.size() >= 2) {
                Direction dir = DirectionFromTo(segments[1], segments[0]);
                rotation = HeadRotation(dir);       // <-- dùng HeadRotation
            } else {
                rotation = HeadRotation(snake.GetCurrentDirection());
            }

        } else if (i == lastIndex) {
            tex = &assets.GetTexture("snake_tail");
            Direction dir = DirectionFromTo(segments[lastIndex - 1], segments[lastIndex]);
            rotation = TailRotation(dir);            // <-- dùng TailRotation (offset khác đầu)

        } else {
            Direction dirIn  = DirectionFromTo(segments[i], segments[i - 1]);
            Direction dirOut = DirectionFromTo(segments[i], segments[i + 1]);

            if (IsStraight(dirIn, dirOut)) {
                tex = &assets.GetTexture("snake_body_straight");
                rotation = StraightRotation(dirIn);
            } else {
                tex = &assets.GetTexture("snake_body_corner");
                rotation = CornerRotation(dirIn, dirOut);
            }
        }

        float scale = (float)cellSize / TILE_SOURCE_SIZE;
        Vector2 pos = { segments[i].x * cellSize, segments[i].y * cellSize };

        Rectangle source = { 0, 0, (float)TILE_SOURCE_SIZE, (float)TILE_SOURCE_SIZE };
        Rectangle dest = { pos.x + cellSize / 2.0f, pos.y + cellSize / 2.0f,
                            (float)cellSize, (float)cellSize };
        Vector2 origin = { cellSize / 2.0f, cellSize / 2.0f };

        DrawTexturePro(*tex, source, dest, origin, rotation, WHITE);
    }
}