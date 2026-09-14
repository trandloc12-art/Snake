#pragma once
#include "Snake/Snake.h"
#include "core/AssetManager.h"
#include "core/Constants.h"
#include <algorithm>   // cho std::clamp
#include <string>

/// Chỉ lo việc VẼ con rắn lên màn hình — không giữ, không thay đổi dữ liệu của Snake.
class SnakeRenderer {
public:
    explicit SnakeRenderer(const AssetManager& assets);

    void Draw(const Snake& snake, int cellSize, float moveAlpha) const;

    /// Đổi skin đang dùng để vẽ rắn (vd "classic", "neon") — chỉ đổi tiền tố tên
    /// texture, KHÔNG load/unload gì (AssetManager đã load sẵn mọi skin lúc LoadAll()).
    void SetSkin(const std::string& skinName);

private:
    const AssetManager& assets; // chỉ giữ tham chiếu, không sở hữu texture
    std::string skinPrefix = std::string(DEFAULT_SNAKE_SKIN) + "_";
};