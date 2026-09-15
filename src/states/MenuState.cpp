#include "states/MenuState.h"
#include "Game.h"
#include "core/Constants.h"
#include "enums/GameState.h"
#include "raylib.h"
#include <cmath>
#include <string>

namespace {
    // Chọn khung hình hiện tại cho background menu - chạy LIÊN TỤC theo thời gian
    // thực tuyệt đối (giống ComputeHeadAnimFrame của SnakeRenderer), không phụ thuộc
    // bất kỳ sự kiện game nào (menu không có khái niệm "di chuyển"/"spawn").
    int ComputeMenuBackgroundFrame(double time, int frameCount, float fps) {
        int frame = static_cast<int>(std::floor(time * fps));
        frame %= frameCount;
        if (frame < 0) frame += frameCount;
        return frame;
    }
}

MenuState::MenuState(Game& game) : game(game) {}

void MenuState::Init() {
    selectedOption = 0; // luôn về lựa chọn đầu tiên khi vào lại Menu
    const AssetManager& assets = game.GetAssets();

    titleTexture = &assets.GetTexture("logo_snake");

    const Texture2D& choiIdle      = assets.GetTexture("btn_choi_game_idle");
    const Texture2D& choiSelected  = assets.GetTexture("btn_choi_game_selected");
    const Texture2D& skinIdle      = assets.GetTexture("btn_chon_skin_idle");      // MỚI
    const Texture2D& skinSelected  = assets.GetTexture("btn_chon_skin_selected");  // MỚI
    const Texture2D& tuyIdle       = assets.GetTexture("btn_tuy_chon_idle");
    const Texture2D& tuySelected   = assets.GetTexture("btn_tuy_chon_selected");
    const Texture2D& thoatIdle     = assets.GetTexture("btn_thoat_idle");
    const Texture2D& thoatSelected = assets.GetTexture("btn_thoat_selected");

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    titlePosition = { screenW / 2.0f - titleTexture->width / 2.0f, 100.0f };

    float spacing = 20.0f;
    float startY = screenH / 2.0f - 70.0f;

    float y = startY;
    buttons[0].Init(choiIdle, choiSelected,   { screenW / 2.0f - choiIdle.width  / 2.0f, y });
    y += choiIdle.height + spacing;
    buttons[1].Init(skinIdle, skinSelected,   { screenW / 2.0f - skinIdle.width  / 2.0f, y }); // MỚI
    y += skinIdle.height + spacing;
    buttons[2].Init(tuyIdle, tuySelected,     { screenW / 2.0f - tuyIdle.width   / 2.0f, y });
    y += tuyIdle.height + spacing;
    buttons[3].Init(thoatIdle, thoatSelected, { screenW / 2.0f - thoatIdle.width / 2.0f, y });
}

// MenuState.cpp — Update(), thêm dòng xử lý ESC
void MenuState::Update() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game.RequestQuit();
        return;
    }

    if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % 4;
    if (IsKeyPressed(KEY_UP))   selectedOption = (selectedOption + 3) % 4;
    
    // Chuột: rê tới nút nào thì đồng bộ selectedOption theo nút đó,
    // để bàn phím và chuột luôn "hiểu" cùng 1 lựa chọn hiện tại.
    for (int i = 0; i < (int)buttons.size(); i++) {
        buttons[i].UpdateHover();
        if (buttons[i].IsHovered()) {
            selectedOption = i;
        }
    }

    bool confirmed = IsKeyPressed(KEY_ENTER) || buttons[selectedOption].IsClicked();

    if (confirmed) {
        switch (selectedOption) {
            case 0: game.ChangeState(GameState::LEVEL_SELECT); break;
            case 1: game.ChangeState(GameState::SKIN_SELECT);  break; // MỚI
            case 2: game.ChangeState(GameState::LEVEL_EDITOR); break;
            case 3: game.RequestQuit(); break;
        }
    }
}

void MenuState::Draw() {
    const AssetManager& assets = game.GetAssets();

    // Background menu giờ là animation 8 khung (xem AssetManager::LoadAll, key
    // "menu_background_0".."menu_background_7"). Chạy liên tục theo thời gian thực,
    // không phụ thuộc lựa chọn/tương tác của người chơi trong menu.
    int bgFrame = ComputeMenuBackgroundFrame(GetTime(), MENU_BACKGROUND_ANIM_FRAMES, MENU_BACKGROUND_ANIM_FPS);
    const Texture2D& background = assets.GetTexture("menu_background_" + std::to_string(bgFrame));
    DrawTexture(background, 0, 0, WHITE);

    DrawTextureV(*titleTexture, titlePosition, WHITE);

    for (int i = 0; i < (int)buttons.size(); i++) {
        buttons[i].Draw(i == selectedOption);
    }

    const char* hint = "Dung phim UP/DOWN de chon, ENTER de xac nhan, hoac dung chuot";
    DrawText(hint, GetScreenWidth() / 2 - MeasureText(hint, 16) / 2, GetScreenHeight() - 60, 16, GRAY);
}