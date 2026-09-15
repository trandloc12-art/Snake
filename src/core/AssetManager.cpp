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

    for (int s = 0; s < SNAKE_SKIN_COUNT; s++) {
        LoadSkinAnimations(SNAKE_SKINS[s], animationsDir);
    }

    // Các file không thuộc rắn: vẫn nằm trực tiếp trong TEXTURES_DIRECTORY như cũ.
    LoadTextureChecked("wall",               texturesDir + "wall.png");
    LoadTextureChecked("food",               texturesDir + "food.png");

    std::string backgroundDir = texturesDir + "background/";
    LoadTextureChecked("playing_background", backgroundDir + "playing_background.png");

    // menu_background giờ là animation 8 khung, nằm trong
    // textures/background/menu_background_animation/menu_background_0.png .. _7.png
    // Key lưu trong bộ nhớ: "menu_background_0" .. "menu_background_7".
    LoadAnimationFramesRaw("menu_background", backgroundDir, "menu_background",
                            MENU_BACKGROUND_ANIM_FRAMES);

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
    LoadTextureChecked("btn_chon_skin_idle",     uiDir + "btn_chon_skin_idle.png",     false);
    LoadTextureChecked("btn_chon_skin_selected", uiDir + "btn_chon_skin_selected.png", false);
    LoadTextureChecked("logo_snake",             uiDir + "logo_snake.png",             false);
    LoadTextureChecked("title_game_over",        uiDir + "title_game_over.png",        false);
    LoadTextureChecked("sprite_dead_snake",      uiDir + "sprite_dead_snake.png",       false);
}

void AssetManager::LoadSkinAnimations(const std::string& skinName, const std::string& animationsDir) {
    // vd skinName = "neon" -> skinDir = "assets/animations/neon/"
    //              prefix  = "neon_"  -> texture key "neon_snake_head_0", v.v.
    std::string skinDir = animationsDir + skinName + "/";
    std::string prefix  = skinName + "_";

    LoadAnimationFrames(prefix + "snake_body",   skinDir, SNAKE_ANIM_FRAMES_BODY);
    LoadAnimationFrames(prefix + "snake_corner", skinDir, SNAKE_ANIM_FRAMES_BODY);
    LoadAnimationFrames(prefix + "snake_tail",   skinDir, SNAKE_ANIM_FRAMES_BODY);
    LoadAnimationFrames(prefix + "snake_head",   skinDir, SNAKE_ANIM_FRAMES_HEAD); // đầu có số khung riêng (4)
}

void AssetManager::LoadAnimationFrames(const std::string& name, const std::string& dir, int frameCount) {
    // "name" đã có sẵn tiền tố skin (vd "neon_snake_head"), nhưng tên THƯ MỤC CON và
    // TÊN FILE bên trong vẫn theo đúng chuẩn cũ, không có tiền tố skin - vì skin đã
    // được tách thành 1 cấp thư mục cha riêng (skinDir) rồi. Cần tách phần tên gốc
    // (bỏ tiền tố skin) để ghép đúng đường dẫn thư mục/file.
    size_t underscorePos = name.find('_');
    std::string rawName = (underscorePos != std::string::npos) ? name.substr(underscorePos + 1) : name;
    // rawName giờ là "snake_head" (bỏ "neon_" ở đầu)

    LoadAnimationFramesRaw(name, dir, rawName, frameCount);
}

void AssetManager::LoadAnimationFramesRaw(const std::string& key, const std::string& dir,
                                           const std::string& rawName, int frameCount) {
    // Phần loop-load DÙNG CHUNG cho mọi loại animation (skin rắn lẫn background menu).
    // Khác với LoadAnimationFrames(), hàm này KHÔNG tự suy ra rawName bằng cách cắt bỏ
    // tiền tố trước dấu '_' đầu tiên - rawName được truyền thẳng vào, nên dùng an toàn
    // cho các tên có dấu '_' bên trong nhưng không mang ý nghĩa "tiền tố skin" (vd
    // "menu_background" không bị hiểu nhầm là skin "menu" + rawName "background").
    for (int frame = 0; frame < frameCount; frame++) {
        std::string frameKey = key + "_" + std::to_string(frame); // vd "menu_background_0"
        std::string path = dir + rawName + "_animation/" + rawName + "_" + std::to_string(frame) + ".png";
        LoadTextureChecked(frameKey, path);
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