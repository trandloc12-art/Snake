#include "states/MenuState.h"
#include "Game.h"
#include "enums/GameState.h"
#include "raylib.h"

MenuState::MenuState(Game& game) : game(game) {}

void MenuState::Init() {
    selectedOption = 0; // luôn về lựa chọn đầu tiên khi vào lại Menu
    const AssetManager& assets = game.GetAssets();

    titleTexture = &assets.GetTexture("logo_snake");

    const Texture2D& choiIdle      = assets.GetTexture("btn_choi_game_idle");
    const Texture2D& choiSelected  = assets.GetTexture("btn_choi_game_selected");
    const Texture2D& tuyIdle       = assets.GetTexture("btn_tuy_chon_idle");
    const Texture2D& tuySelected   = assets.GetTexture("btn_tuy_chon_selected");
    const Texture2D& thoatIdle     = assets.GetTexture("btn_thoat_idle");
    const Texture2D& thoatSelected = assets.GetTexture("btn_thoat_selected");

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    titlePosition = { screenW / 2.0f - titleTexture->width / 2.0f, 100.0f };

    float spacing = 20.0f;
    float startY = screenH / 2.0f - 20.0f; // căn quanh giữa màn hình, dưới logo

    float y = startY;
    buttons[0].Init(choiIdle, choiSelected,   { screenW / 2.0f - choiIdle.width  / 2.0f, y });
    y += choiIdle.height + spacing;
    buttons[1].Init(tuyIdle, tuySelected,     { screenW / 2.0f - tuyIdle.width   / 2.0f, y });
    y += tuyIdle.height + spacing;
    buttons[2].Init(thoatIdle, thoatSelected, { screenW / 2.0f - thoatIdle.width / 2.0f, y });
}

// MenuState.cpp — Update(), thêm dòng xử lý ESC
void MenuState::Update() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game.RequestQuit();
        return;
    }

    if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % 3;
    if (IsKeyPressed(KEY_UP))   selectedOption = (selectedOption + 2) % 3;
    
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
            case 1: game.ChangeState(GameState::LEVEL_EDITOR); break;
            case 2: game.RequestQuit(); break;
        }
    }
}

void MenuState::Draw() {
    DrawTextureV(*titleTexture, titlePosition, WHITE);

    for (int i = 0; i < (int)buttons.size(); i++) {
        buttons[i].Draw(i == selectedOption);
    }

    const char* hint = "Dung phim UP/DOWN de chon, ENTER de xac nhan, hoac dung chuot";
    DrawText(hint, GetScreenWidth() / 2 - MeasureText(hint, 16) / 2, GetScreenHeight() - 60, 16, GRAY);
}