// SnakeRenderer.cpp
#include "Snake/SnakeRenderer.h"
#include "core/Constants.h"
#include <cassert>
#include <algorithm>

namespace {
    Direction DirectionFromTo(Vector2 from, Vector2 to) {
        if (to.x > from.x) return Direction::RIGHT;
        if (to.x < from.x) return Direction::LEFT;
        if (to.y > from.y) return Direction::DOWN;
        return Direction::UP;
    }

    // Đầu vẽ sẵn chỉ LÊN (UP) = 0°. Xoay CW: UP -> RIGHT -> DOWN -> LEFT = 0/90/180/270.
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

    // Thân thẳng vẽ sẵn DỌC (nối UP-DOWN) = 0°.
    float StraightRotation(Direction dirIn) {
        bool vertical = (dirIn == Direction::UP || dirIn == Direction::DOWN);
        return vertical ? 0.0f : 90.0f;
    }

    // Góc cua vẽ sẵn nối LEFT + DOWN = 0°.
    float CornerRotation(Direction dirIn, Direction dirOut) {
        bool hasLeft  = (dirIn == Direction::LEFT  || dirOut == Direction::LEFT);
        bool hasDown  = (dirIn == Direction::DOWN  || dirOut == Direction::DOWN);
        bool hasUp    = (dirIn == Direction::UP    || dirOut == Direction::UP);
        bool hasRight = (dirIn == Direction::RIGHT || dirOut == Direction::RIGHT);

        if (hasLeft  && hasDown)  return 0.0f;
        if (hasUp    && hasLeft)  return 90.0f;
        if (hasRight && hasUp)    return 180.0f;
        return 270.0f;
    }
}

SnakeRenderer::SnakeRenderer(const AssetManager& assets) : assets(assets) {}

void SnakeRenderer::Draw(const Snake& snake, int cellSize, float moveAlpha) const {
    const auto& segments = snake.GetSegments();
    const auto& prevSegments = snake.GetPrevSegments();
    float alpha = std::clamp(moveAlpha, 0.0f, 1.0f);

    size_t lastIndex = segments.size() - 1;

    for (size_t i = 0; i < segments.size(); i++) {
        const Texture2D* tex = nullptr;
        float rotation = 0.0f;
        
        bool interpolate = false;

        if (i == 0) {
            tex = &assets.GetTexture("snake_head");
            interpolate = true;

            if (segments.size() >= 2) {
                Direction dir = DirectionFromTo(segments[1], segments[0]);
                rotation = HeadRotation(dir);
            } else {
                rotation = HeadRotation(snake.GetCurrentDirection());
            }

        } else if (i == lastIndex) {
            tex = &assets.GetTexture("snake_tail");
            Direction dir = DirectionFromTo(segments[lastIndex - 1], segments[lastIndex]);
            rotation = TailRotation(dir);

            if (segments.size() >= 3) {
                // Trạng thái đốt gần đuôi ở tick HIỆN TẠI
                Direction dirIn  = DirectionFromTo(segments[lastIndex - 1], segments[lastIndex - 2]);
                Direction dirOut = DirectionFromTo(segments[lastIndex - 1], segments[lastIndex]);
                bool nearTailIsCorner = !IsStraight(dirIn, dirOut);

                // Trạng thái đốt gần đuôi ở tick TRƯỚC
                bool nearTailWasCorner = false;
                if (prevSegments.size() > lastIndex) {
                    Direction dirInPrev  = DirectionFromTo(prevSegments[lastIndex - 1], prevSegments[lastIndex - 2]);
                    Direction dirOutPrev = DirectionFromTo(prevSegments[lastIndex - 1], prevSegments[lastIndex]);
                    nearTailWasCorner = !IsStraight(dirInPrev, dirOutPrev);
                }

                // Chỉ tắt nội suy đúng lúc chuyển tiếp: trước là rẽ, giờ đã thẳng
                bool justStraightened = nearTailWasCorner && !nearTailIsCorner;
                interpolate = !justStraightened;
            } else {
                interpolate = IsStraight(dir, snake.GetCurrentDirection());
            }
        } else {
            Direction dirIn  = DirectionFromTo(segments[i], segments[i - 1]);
            Direction dirOut = DirectionFromTo(segments[i], segments[i + 1]);
            bool isCorner = !IsStraight(dirIn, dirOut);

            // Trạng thái đốt này ở tick TRƯỚC (nếu dữ liệu hợp lệ, không grow lệch index)
            bool wasCorner = false;
            if (prevSegments.size() > i + 1) {
                Direction dirInPrev  = DirectionFromTo(prevSegments[i], prevSegments[i - 1]);
                Direction dirOutPrev = DirectionFromTo(prevSegments[i], prevSegments[i + 1]);
                wasCorner = !IsStraight(dirInPrev, dirOutPrev);
            }

            // Khúc cua VỪA xuất hiện ở đốt này (trước thẳng, giờ rẽ) -> tắt nội suy đúng lúc này
            bool justCornered = !wasCorner && isCorner;
            interpolate = !justCornered;

            tex = isCorner ? &assets.GetTexture("snake_body_corner")
                            : &assets.GetTexture("snake_body_straight");
            rotation = isCorner ? CornerRotation(dirIn, dirOut) : StraightRotation(dirIn);
        }

        Vector2 pos;
        if (interpolate) {
            Vector2 prev = (i < prevSegments.size()) ? prevSegments[i] : segments[i];
            float interpX = prev.x + (segments[i].x - prev.x) * alpha;
            float interpY = prev.y + (segments[i].y - prev.y) * alpha;
            pos = { interpX * cellSize, interpY * cellSize };
        } else {
            pos = { segments[i].x * cellSize, segments[i].y * cellSize };
        }

        Rectangle source = { 0, 0, (float)TILE_SOURCE_SIZE, (float)TILE_SOURCE_SIZE };
        Rectangle dest = { pos.x + cellSize / 2.0f, pos.y + cellSize / 2.0f,
                            (float)cellSize, (float)cellSize };
        Vector2 origin = { cellSize / 2.0f, cellSize / 2.0f };

        DrawTexturePro(*tex, source, dest, origin, rotation, WHITE);
    }
}