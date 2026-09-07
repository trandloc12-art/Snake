#pragma once
#include "Snake/Snake.h"
#include "core/AssetManager.h"
#include <algorithm>   // cho std::clamp

/// Chỉ lo việc VẼ con rắn lên màn hình — không giữ, không thay đổi dữ liệu của Snake.
class SnakeRenderer {
public:
    explicit SnakeRenderer(const AssetManager& assets);

    void Draw(const Snake& snake, int cellSize, float moveAlpha) const;

private:
    const AssetManager& assets; // chỉ giữ tham chiếu, không sở hữu texture
};