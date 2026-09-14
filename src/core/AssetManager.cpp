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

    std::string uiDir = texturesDir + "ui/";
    LoadTextureChecked("btn_choi_game_idle",     uiDir + "btn_choi_game_idle.png",     false);
    LoadTextureChecked("btn_choi_game_selected", uiDir + "btn_choi_game_selected.png", false);
    LoadTextureChecked("btn_tuy_chon_idle",      uiDir + "btn_tuy_chon_idle.png",      false);
    LoadTextureChecked("btn_tuy_chon_selected",  uiDir + "btn_tuy_chon_selected.png",  false);
    LoadTextureChecked("btn_thoat_idle",         uiDir + "btn_thoat_idle.png",         false);
    LoadTextureChecked("btn_thoat_selected",     uiDir + "btn_thoat_selected.png",     false);
    LoadTextureChecked("btn_choi_lai_idle",      uiDir + "btn_choi_lai_idle.png",      false);
    LoadTextureChecked("btn_choi_lai_selected",  uiDir + "btn_choi_lai_selected.png",  false);
    LoadTextureChecked("btn_ve_menu_idle",       uiDir + "btn_ve_menu_idle.png",       false);
    LoadTextureChecked("btn_ve_menu_selected",   uiDir + "btn_ve_menu_selected.png",   false);
    LoadTextureChecked("logo_snake",             uiDir + "logo_snake.png",             false);
    LoadTextureChecked("title_game_over",        uiDir + "title_game_over.png",        false);
    LoadTextureChecked("sprite_dead_snake",      uiDir + "sprite_dead_snake.png",       false);
}

void AssetManager::LoadAnimationFrames(const std::string& name, const std::string& dir, int frameCount) {
    for (int frame = 0; frame < frameCount; frame++) {
        std::string key = name + "_" + std::to_string(frame);
        std::string path = dir + name + "_animation/" + key + ".png";
        LoadTextureChecked(key, path);
    }
}

void AssetManager::LoadTextureChecked(const std::string& name, const std::string& path, bool checkSize) {
    Texture2D tex = LoadTexture(path.c_str());

    if (tex.id == 0) {
        std::cerr << "AssetManager: LOI khong load duoc file '" << path << "'\n";
        textures[name] = tex;
        return;
    }

    SetTextureFilter(tex, TEXTURE_FILTER_POINT);

    if (checkSize && (tex.width != TILE_SOURCE_SIZE || tex.height != TILE_SOURCE_SIZE)) {
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