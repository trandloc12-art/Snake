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
    LoadTextureChecked("playing_background", dir + "playing_background.png");

    // MỚI: asset giao diện (nút, logo, tiêu đề) — không theo chuẩn kích thước tile
    // nên checkSize = false. Đặt trong thư mục con "ui/" cho gọn, cậu nhớ tạo thư mục
    // này và copy 13 file PNG vào đó.
    std::string uiDir = dir + "ui/";
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