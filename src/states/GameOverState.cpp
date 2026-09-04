#include "states/GameOverState.h"
#include "Game.h"
#include "enums/GameState.h"
#include "raylib.h"

GameOverState::GameOverState(Game& game) : game(game) {}

void GameOverState::Init() {}

void GameOverState::Update() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
        game.ChangeState(GameState::MENU);
    }
    
    if (IsKeyPressed(KEY_ENTER)) {
        game.ChangeState(GameState::MENU);
    }
}

void GameOverState::Draw() {
    DrawText("GAME OVER", 280, 220, 40, RED);
    DrawText(TextFormat("Diem cua ban: %d", game.GetLastScore()), 300, 280, 24, BLACK);
    DrawText("Nhan ENTER de quay lai Menu", 250, 340, 18, GRAY);
}