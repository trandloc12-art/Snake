#pragma once
#include <unordered_map>
#include <string>
#include "raylib.h"

/// Load và quản lý tập trung toàn bộ texture của game.
/// Chỉ nên có 1 instance duy nhất, tạo ở Game và truyền tham chiếu cho các nơi cần vẽ.
class AssetManager {
public:
    /// Load tất cả texture cần dùng trong game — gọi 1 LẦN DUY NHẤT lúc khởi động.
    void LoadAll();

    /// Lấy texture theo tên (đã load sẵn) để vẽ. Trả về texture rỗng nếu tên không tồn tại.
    const Texture2D& GetTexture(const std::string& name) const;

    /// Giải phóng toàn bộ texture — gọi 1 LẦN DUY NHẤT lúc thoát game.
    void UnloadAll();

private:
    std::unordered_map<std::string, Texture2D> textures;
};