#include "states/GameOverState.h"
#include "Game.h"
#include "enums/GameState.h"
#include "raylib.h"

GameOverState::GameOverState(Game& game) : game(game) {}

void GameOverState::Init() {
    selectedOption = 0; // luôn về "Choi lai" mỗi khi vào lại màn hình này
}

void GameOverState::Update() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game.ChangeState(GameState::MENU);
        return;
    }

    if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % 2;
    if (IsKeyPressed(KEY_UP))   selectedOption = (selectedOption + 1) % 2; // chỉ 2 lựa chọn nên UP/DOWN đảo qua lại như nhau

    if (IsKeyPressed(KEY_ENTER)) {
        switch (selectedOption) {
            case 0: game.ChangeState(GameState::PLAYING); break; // Choi lai: dùng lại selectedLevelPath hiện tại
            case 1: game.ChangeState(GameState::MENU);    break; // Ve Menu
        }
    }
}

void GameOverState::Draw() {
    const char* options[2] = { "Choi lai", "Ve Menu" };

    DrawText("GAME OVER", 280, 220, 40, RED);
    DrawText(TextFormat("Diem cua ban: %d", game.GetLastScore()), 300, 280, 24, BLACK);

    for (int i = 0; i < 2; i++) {
        Color color = (i == selectedOption) ? RED : BLACK;
        DrawText(options[i], 300, 340 + i * 40, 22, color);
    }

    DrawText("Dung phim UP/DOWN de chon, ENTER de xac nhan", 200, 460, 16, GRAY);
}