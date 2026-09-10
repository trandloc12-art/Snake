#include "states/GameOverState.h"
#include "Game.h"
#include "enums/GameState.h"
#include "core/AssetManager.h"
#include "raylib.h"

GameOverState::GameOverState(Game& game) : game(game) {}

void GameOverState::Init() {
    selectedOption = 0;

    const AssetManager& assets = game.GetAssets();

    titleTexture = &assets.GetTexture("title_game_over");
    deadSnakeTexture = &assets.GetTexture("sprite_dead_snake");

    const Texture2D& choiLaiIdle     = assets.GetTexture("btn_choi_lai_idle");
    const Texture2D& choiLaiSelected = assets.GetTexture("btn_choi_lai_selected");
    const Texture2D& veMenuIdle      = assets.GetTexture("btn_ve_menu_idle");
    const Texture2D& veMenuSelected  = assets.GetTexture("btn_ve_menu_selected");

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    titlePosition = { screenW / 2.0f - titleTexture->width / 2.0f, 140.0f };
    deadSnakePosition = {
        screenW / 2.0f - deadSnakeTexture->width / 2.0f,
        titlePosition.y + titleTexture->height + 70.0f
    };

    float spacing = 20.0f;
    float startY = screenH / 2.0f + 20.0f;

    buttons[0].Init(choiLaiIdle, choiLaiSelected, { screenW / 2.0f - choiLaiIdle.width / 2.0f, startY });
    buttons[1].Init(veMenuIdle, veMenuSelected,
                     { screenW / 2.0f - veMenuIdle.width / 2.0f, startY + choiLaiIdle.height + spacing });
}

void GameOverState::Update() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game.ChangeState(GameState::MENU);
        return;
    }

    if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % 2;
    if (IsKeyPressed(KEY_UP))   selectedOption = (selectedOption + 1) % 2;

    for (int i = 0; i < (int)buttons.size(); i++) {
        buttons[i].UpdateHover();
        if (buttons[i].IsHovered()) {
            selectedOption = i;
        }
    }

    bool confirmed = IsKeyPressed(KEY_ENTER) || buttons[selectedOption].IsClicked();

    if (confirmed) {
        switch (selectedOption) {
            case 0: game.ChangeState(GameState::PLAYING); break;
            case 1: game.ChangeState(GameState::MENU);    break;
        }
    }
}

void GameOverState::Draw() {
    DrawTextureV(*titleTexture, titlePosition, WHITE);

    const char* scoreText = TextFormat("Diem cua ban: %d", game.GetLastScore());
    DrawText(scoreText,
             GetScreenWidth() / 2 - MeasureText(scoreText, 24) / 2,
             (int)(titlePosition.y + titleTexture->height + 20),
             24, BLACK);
    DrawTextureV(*deadSnakeTexture, deadSnakePosition, WHITE);
    for (int i = 0; i < (int)buttons.size(); i++) {
        buttons[i].Draw(i == selectedOption);
    }

    const char* hint = "Dung phim UP/DOWN de chon, ENTER de xac nhan, hoac dung chuot";
    DrawText(hint, GetScreenWidth() / 2 - MeasureText(hint, 16) / 2, GetScreenHeight() - 60, 16, GRAY);
}