#include "states/MenuState.h"
#include "Game.h"
#include "enums/GameState.h"
#include "raylib.h"

MenuState::MenuState(Game& game) : game(game) {}

void MenuState::Init() {
    selectedOption = 0; // luôn về lựa chọn đầu tiên khi vào lại Menu
}

// MenuState.cpp — Update(), thêm dòng xử lý ESC
void MenuState::Update() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game.RequestQuit();
        return;
    }

    if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % 3;
    if (IsKeyPressed(KEY_UP))   selectedOption = (selectedOption + 2) % 3;

    if (IsKeyPressed(KEY_ENTER)) {
        switch (selectedOption) {
            case 0: game.ChangeState(GameState::LEVEL_SELECT); break;
            case 1: game.ChangeState(GameState::LEVEL_EDITOR); break;
            case 2: game.RequestQuit(); break; // lựa chọn "Thoát" trong menu cũng gọi hàm này
        }
    }
}

void MenuState::Draw() {
    const char* options[3] = { "Choi", "Sua man (Editor)", "Thoat (ESC)" };

    DrawText("SNAKE GAME", 250, 100, 40, DARKGREEN);

    for (int i = 0; i < 3; i++) {
        Color color = (i == selectedOption) ? RED : BLACK;
        DrawText(options[i], 300, 220 + i * 40, 24, color);
    }

    DrawText("Dung phim UP/DOWN de chon, ENTER de xac nhan", 200, 500, 16, GRAY);
}