// SnakeRenderer.cpp
#include "Snake/SnakeRenderer.h"
#include "core/Constants.h"
#include <cassert>
#include <cmath>
#include <string>

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

    // Chọn khung hình animation hiện tại cho 1 đốt, dựa theo:
    // - thời gian thực (time, giây) trừ đi thời điểm đốt đó được SINH RA (spawnTime)
    // - fps: tốc độ chạy khung hình
    // Đốt càng gần ĐẦU thì spawnTime càng gần "now" -> (time - spawnTime) nhỏ -> frame
    // sớm hơn; đốt gần ĐUÔI đã "sống" lâu hơn -> frame trễ hơn -> tạo cảm giác hoạ văn
    // "chảy" dọc thân từ đầu xuống đuôi, giống hiệu ứng cũ nhưng KHÔNG còn phụ thuộc vào
    // segmentIndex (vốn bị dịch +1 mỗi khi rắn di chuyển do push_front đầu mới) — nên
    // không còn bị giật/lệch phase giữa các bước đi.
    int ComputeAnimFrame(double time, double spawnTime, int frameCount, float fps) {
        float framePos = static_cast<float>((time - spawnTime) * fps);
        int frame = static_cast<int>(std::floor(framePos));
        frame %= frameCount;
        if (frame < 0) frame += frameCount; // % trong C++ có thể trả âm, chuẩn hoá lại về [0, frameCount)
        return frame;
    }
}

SnakeRenderer::SnakeRenderer(const AssetManager& assets) : assets(assets) {}

void SnakeRenderer::Draw(const Snake& snake, int cellSize, float moveAlpha) const {
    // moveAlpha KHÔNG dùng để nội suy vị trí - mọi đốt snap đứng yên đúng ô lưới hiện
    // tại (segments[i] * cellSize). Quyết định giữ snap-grid (không nội suy) vì nội suy
    // vị trí gây gãy khớp hình học ở góc cua (corner sprite chỉ đúng hình tại thời điểm
    // bẻ góc hoàn tất, không có texture nào khớp giữa chừng). Cảm giác "di chuyển mượt"
    // đến hoàn toàn từ animation flipbook chạy theo đồng hồ thực. Giữ tham số này lại để
    // không đổi chữ ký hàm Draw().
    (void)moveAlpha;

    const auto& segments = snake.GetSegments();
    const auto& spawnTimes = snake.GetSegmentSpawnTimes();
    if (segments.empty()) return;

    // segments và spawnTimes phải luôn cùng kích thước (xem Snake::Move/Init/InitFromLevel).
    assert(segments.size() == spawnTimes.size());

    double time = GetTime();
    size_t lastIndex = segments.size() - 1;

    auto drawTile = [&](Vector2 pos, const Texture2D& t, float rot, Color tint) {
        Rectangle source = { 0, 0, (float)TILE_SOURCE_SIZE, (float)TILE_SOURCE_SIZE };
        Rectangle dest = { pos.x + cellSize / 2.0f, pos.y + cellSize / 2.0f,
                            (float)cellSize, (float)cellSize };
        Vector2 origin = { cellSize / 2.0f, cellSize / 2.0f };
        DrawTexturePro(t, source, dest, origin, rot, tint);
    };

    for (size_t i = 0; i < segments.size(); i++) {
        // Vị trí: luôn snap đúng ô lưới hiện tại, không nội suy prevSegments -> segments.
        Vector2 pos = { segments[i].x * cellSize, segments[i].y * cellSize };

        const Texture2D* tex = nullptr;
        float rotation = 0.0f;

        if (i == 0) {
            Direction dir = (segments.size() >= 2)
                ? DirectionFromTo(segments[1], segments[0])
                : snake.GetCurrentDirection();
            rotation = HeadRotation(dir);

            int frame = ComputeAnimFrame(time, spawnTimes[i], SNAKE_ANIM_FRAMES_BODY, SNAKE_ANIM_FPS);
            tex = &assets.GetTexture("snake_head_" + std::to_string(frame));

        } else if (i == lastIndex) {
            Direction dir = DirectionFromTo(segments[lastIndex - 1], segments[lastIndex]);
            rotation = TailRotation(dir);

            int frame = ComputeAnimFrame(time, spawnTimes[i], SNAKE_ANIM_FRAMES_BODY, SNAKE_ANIM_FPS);
            tex = &assets.GetTexture("snake_tail_" + std::to_string(frame));

        } else {
            Direction dirIn  = DirectionFromTo(segments[i], segments[i - 1]);
            Direction dirOut = DirectionFromTo(segments[i], segments[i + 1]);

            if (IsStraight(dirIn, dirOut)) {
                rotation = StraightRotation(dirIn);

                int frame = ComputeAnimFrame(time, spawnTimes[i], SNAKE_ANIM_FRAMES_BODY, SNAKE_ANIM_FPS);
                tex = &assets.GetTexture("snake_body_" + std::to_string(frame));
            } else {
                rotation = CornerRotation(dirIn, dirOut);

                // Góc cua dùng chung kiến trúc animation 8 khung như thân thẳng/đầu/đuôi
                // (asset thật nằm trong thư mục snake_corner/, tên file
                // snake_corner_0.png..snake_corner_7.png).
                int frame = ComputeAnimFrame(time, spawnTimes[i], SNAKE_ANIM_FRAMES_BODY, SNAKE_ANIM_FPS);
                tex = &assets.GetTexture("snake_corner_" + std::to_string(frame));
            }
        }

        drawTile(pos, *tex, rotation, WHITE);
    }
}