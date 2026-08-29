#include "snake/SnakeRenderer.h"

SnakeRenderer::SnakeRenderer(const AssetManager& assets) : assets(assets) {}

void SnakeRenderer::Draw(const Snake& snake, int cellSize) const {
    const auto& segments = snake.GetSegments();

    for (size_t i = 0; i < segments.size(); i++) {
        const Texture2D& tex = (i == 0)
            ? assets.GetTexture("snake_head")
            : assets.GetTexture("snake_body");

        float scale = (float)cellSize / tex.width;
        Vector2 pos = { segments[i].x * cellSize, segments[i].y * cellSize };

        DrawTextureEx(tex, pos, 0.0f, scale, WHITE);
    }
}