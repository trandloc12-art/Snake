#include "core/AssetManager.h"
#include "core/Constants.h"
#include <iostream>
#include <cassert>

AssetManager::~AssetManager() {
    UnloadAll();
}

void AssetManager::LoadAll() {
    std::string dir = TEXTURES_DIRECTORY;
    LoadTextureChecked("snake_head",          dir + "snake_head.png");
    LoadTextureChecked("snake_body_straight", dir + "snake_body_straight.png");
    LoadTextureChecked("snake_body_corner",   dir + "snake_body_corner.png");
    LoadTextureChecked("snake_tail",          dir + "snake_tail.png");
    LoadTextureChecked("wall",                dir + "wall.png");
    LoadTextureChecked("food",                dir + "food.png");
}

void AssetManager::LoadTextureChecked(const std::string& name, const std::string& path) {
    Texture2D tex = LoadTexture(path.c_str());

    if (tex.id == 0) {
        // File không tồn tại / sai đường dẫn -> báo lỗi ngay, tránh lỗi âm thầm
        // trôi tới tận lúc vẽ mới phát hiện.
        std::cerr << "AssetManager: LOI khong load duoc file '" << path << "'\n";
        textures[name] = tex;
        return;
    }

    // Giữ pixel sắc nét khi scale (DrawTexturePro co giãn theo cellSize) -
    // bắt buộc cho phong cách pixel art, tránh bị mờ viền.
    SetTextureFilter(tex, TEXTURE_FILTER_POINT);

    // Cảnh báo nếu ai đó lỡ vẽ sai kích thước chuẩn - không chặn chương trình,
    // chỉ nhắc để bạn kiểm tra lại file ảnh.
    if (tex.width != TILE_SOURCE_SIZE || tex.height != TILE_SOURCE_SIZE) {
        std::cerr << "AssetManager: CANH BAO '" << path << "' kich thuoc "
                   << tex.width << "x" << tex.height
                   << ", khac chuan " << TILE_SOURCE_SIZE << "x" << TILE_SOURCE_SIZE << "\n";
    }

    textures[name] = tex;
}

const Texture2D& AssetManager::GetTexture(const std::string& name) const {
    auto it = textures.find(name);
    if (it == textures.end()) {
        std::cerr << "AssetManager: khong tim thay texture '" << name << "'\n";
        assert(false && "Texture khong ton tai - kiem tra lai ten khi goi GetTexture()");
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