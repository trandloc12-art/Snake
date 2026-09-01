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

    float DirectionToRotation(Direction dir) {
        switch (dir) {
            case Direction::UP:    return 0.0f;
            case Direction::RIGHT: return 90.0f;
            case Direction::DOWN:  return 180.0f;
            case Direction::LEFT:  return 270.0f;
        }
        return 0.0f;
    }

    bool IsStraight(Direction dirIn, Direction dirOut) {
        return (dirIn == Direction::UP && dirOut == Direction::DOWN) ||
               (dirIn == Direction::DOWN && dirOut == Direction::UP) ||
               (dirIn == Direction::LEFT && dirOut == Direction::RIGHT) ||
               (dirIn == Direction::RIGHT && dirOut == Direction::LEFT);
    }

    float StraightRotation(Direction dirIn) {
        bool vertical = (dirIn == Direction::UP || dirIn == Direction::DOWN);
        return vertical ? 0.0f : 90.0f;
    }

    float CornerRotation(Direction dirIn, Direction dirOut) {
        bool hasDown  = (dirIn == Direction::DOWN  || dirOut == Direction::DOWN);
        bool hasRight = (dirIn == Direction::RIGHT || dirOut == Direction::RIGHT);
        bool hasUp    = (dirIn == Direction::UP    || dirOut == Direction::UP);
        bool hasLeft  = (dirIn == Direction::LEFT  || dirOut == Direction::LEFT);

        if (hasDown && hasRight) return 0.0f;
        if (hasDown && hasLeft)  return 90.0f;
        if (hasUp   && hasLeft)  return 180.0f;
        return 270.0f;
    }
}

SnakeRenderer::SnakeRenderer(const AssetManager& assets) : assets(assets) {}

void SnakeRenderer::Draw(const Snake& snake, int cellSize) const {
    // Cho phép cả 2 chiều: phóng to (cellSize là bội số của source)
    // hoặc thu nhỏ (source là bội số của cellSize) - miễn tỷ lệ là số nguyên sạch.
    bool validScale = (cellSize >= TILE_SOURCE_SIZE)
        ? (cellSize % TILE_SOURCE_SIZE == 0)
        : (TILE_SOURCE_SIZE % cellSize == 0);

    assert(validScale &&
           "cellSize va TILE_SOURCE_SIZE phai co ty le la so nguyen (theo 1 trong 2 chieu)");
           
    float scale = (float)cellSize / TILE_SOURCE_SIZE; // luôn là số nguyên: 1.0, 2.0, 3.0...

    const auto& segments = snake.GetSegments();
    size_t lastIndex = segments.size() - 1;

    for (size_t i = 0; i < segments.size(); i++) {
        const Texture2D* tex;
        float rotation = 0.0f;

        if (i == 0) {
            tex = &assets.GetTexture("snake_head");
            if (segments.size() >= 2) {
                Direction dir = DirectionFromTo(segments[1], segments[0]);
                rotation = DirectionToRotation(dir);
            } else {
                rotation = DirectionToRotation(snake.GetCurrentDirection());
            }
        } else if (i == lastIndex) {
            tex = &assets.GetTexture("snake_tail");
            Direction dir = DirectionFromTo(segments[lastIndex - 1], segments[lastIndex]);
            rotation = DirectionToRotation(dir);
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

        Vector2 pos = { segments[i].x * cellSize, segments[i].y * cellSize };

        // source luôn lấy đúng kích thước gốc của ảnh (không co giãn ở bước này)
        Rectangle source = { 0, 0, (float)TILE_SOURCE_SIZE, (float)TILE_SOURCE_SIZE };
        // dest dùng cellSize thật -> đây là nơi phép "scale" thực sự diễn ra
        Rectangle dest = { pos.x + cellSize / 2.0f, pos.y + cellSize / 2.0f,
                            (float)cellSize, (float)cellSize };
        Vector2 origin = { cellSize / 2.0f, cellSize / 2.0f };

        DrawTexturePro(*tex, source, dest, origin, rotation, WHITE);
    }
}