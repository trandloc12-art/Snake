#include "core/AssetManager.h"
#include "core/Constants.h"
#include <iostream>
#include <cassert>

AssetManager::~AssetManager() {
    UnloadAll();
}

void AssetManager::LoadAll() {
    std::string texturesDir   = TEXTURES_DIRECTORY;
    std::string animationsDir = ANIMATIONS_DIRECTORY;

    // Đầu / thân thẳng / đuôi / góc cua: mỗi loại nằm trong 1 THƯ MỤC CON riêng bên trong
    // ANIMATIONS_DIRECTORY, tên thư mục = "<name>_animation" (vd assets/animations/
    // snake_head_animation/snake_head_0.png .. snake_head_7.png) - CHỈ thư mục có hậu tố
    // "_animation", tên FILE bên trong vẫn giữ nguyên như cũ (không có hậu tố này).
    // Mỗi bộ gồm SNAKE_ANIM_FRAMES_BODY khung animation flipbook. Lưu ý: tên của thân
    // thẳng là "snake_body" (không phải "snake_body_straight"), góc cua là "snake_corner"
    // (không phải "snake_body_corner").
    LoadAnimationFrames("snake_head",   animationsDir, SNAKE_ANIM_FRAMES_BODY);
    LoadAnimationFrames("snake_body",   animationsDir, SNAKE_ANIM_FRAMES_BODY);
    LoadAnimationFrames("snake_tail",   animationsDir, SNAKE_ANIM_FRAMES_BODY);
    LoadAnimationFrames("snake_corner", animationsDir, SNAKE_ANIM_FRAMES_BODY);

    // Các file không thuộc rắn: vẫn nằm trực tiếp trong TEXTURES_DIRECTORY như cũ.
    LoadTextureChecked("wall",               texturesDir + "wall.png");
    LoadTextureChecked("food",               texturesDir + "food.png");
    LoadTextureChecked("playing_background", texturesDir + "playing_background.png");
}

void AssetManager::LoadAnimationFrames(const std::string& name, const std::string& dir, int frameCount) {
    for (int frame = 0; frame < frameCount; frame++) {
        std::string key = name + "_" + std::to_string(frame);
        std::string path = dir + name + "_animation/" + key + ".png";
        LoadTextureChecked(key, path);
    }
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