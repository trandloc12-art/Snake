#include "core/AssetManager.h"
#include <iostream>

void AssetManager::LoadAll() {
    // Liệt kê TẤT CẢ texture ở đây — muốn thêm ảnh mới chỉ cần thêm 1 dòng.
    textures["snake_head"] = LoadTexture("assets/textures/snake_head.png");
    textures["snake_body"] = LoadTexture("assets/textures/snake_body.png");
    textures["wall"]       = LoadTexture("assets/textures/wall.png");
    textures["food"]       = LoadTexture("assets/textures/food.png");
}

const Texture2D& AssetManager::GetTexture(const std::string& name) const {
    auto it = textures.find(name);
    if (it == textures.end()) {
        // Không throw exception để tránh crash game — chỉ cảnh báo ra console,
        // và trả về texture đầu tiên tồn tại (tạm) để không bị lỗi truy cập bộ nhớ.
        std::cerr << "AssetManager: khong tim thay texture '" << name << "'\n";
        static Texture2D empty = {};
        return empty;
    }
    return it->second;
}

void AssetManager::UnloadAll() {
    for (auto& pair : textures) {
        UnloadTexture(pair.second);
    }
    textures.clear();
}