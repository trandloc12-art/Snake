#include "states/SkinSelectState.h"
#include "Game.h"
#include "enums/GameState.h"
#include "core/Constants.h"
#include "raylib.h"
#include <string>

SkinSelectState::SkinSelectState(Game& game) : game(game) {}

void SkinSelectState::Init() {
    // Mở màn này với lựa chọn đang hiện là skin ĐANG DÙNG hiện tại, không phải luôn về 0,
    // để người chơi thấy ngay skin nào đang active.
    selectedIndex = 0;
    for (int i = 0; i < SNAKE_SKIN_COUNT; i++) {
        if (game.GetSelectedSkin() == SNAKE_SKINS[i]) {
            selectedIndex = i;
            break;
        }
    }
}

void SkinSelectState::Update() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game.ChangeState(GameState::MENU);
        return;
    }

    if (IsKeyPressed(KEY_DOWN)) selectedIndex = (selectedIndex + 1) % SNAKE_SKIN_COUNT;
    if (IsKeyPressed(KEY_UP))   selectedIndex = (selectedIndex + SNAKE_SKIN_COUNT - 1) % SNAKE_SKIN_COUNT;

    // Chuột: hover qua từng dòng chữ để chọn - toạ độ Y phải khớp CHÍNH XÁC với vòng
    // lặp vẽ trong Draw() (cùng startY, cùng lineHeight) để vùng click đúng vị trí hiển thị.
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    float lineHeight = 50.0f;
    float startY = screenH / 2.0f - (SNAKE_SKIN_COUNT * lineHeight) / 2.0f;

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < SNAKE_SKIN_COUNT; i++) {
        Rectangle row = { screenW / 2.0f - 150.0f, startY + i * lineHeight, 300.0f, lineHeight };
        if (CheckCollisionPointRec(mouse, row)) {
            selectedIndex = i;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                game.SetSelectedSkin(SNAKE_SKINS[i]);
                game.ChangeState(GameState::MENU);
                return;
            }
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        game.SetSelectedSkin(SNAKE_SKINS[selectedIndex]);
        game.ChangeState(GameState::MENU);
    }
}

void SkinSelectState::Draw() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    const char* title = "CHON SKIN";
    int titleFontSize = 40;
    DrawText(title, screenW / 2 - MeasureText(title, titleFontSize) / 2, 100, titleFontSize, DARKGREEN);

    float lineHeight = 50.0f;
    float startY = screenH / 2.0f - (SNAKE_SKIN_COUNT * lineHeight) / 2.0f;

    for (int i = 0; i < SNAKE_SKIN_COUNT; i++) {
        bool isSelected = (i == selectedIndex);
        bool isActive = (game.GetSelectedSkin() == SNAKE_SKINS[i]); // skin ĐANG DÙNG thật sự

        Color color = isSelected ? RED : BLACK;
        std::string label = SNAKE_SKINS[i];
        if (isActive) label += " (dang dung)";

        int fontSize = 24;
        DrawText(label.c_str(),
                 screenW / 2 - MeasureText(label.c_str(), fontSize) / 2,
                 (int)(startY + i * lineHeight),
                 fontSize, color);
    }

    const char* hint = "Dung phim UP/DOWN de chon, ENTER de xac nhan, hoac dung chuot. ESC de quay lai";
    DrawText(hint, screenW / 2 - MeasureText(hint, 16) / 2, screenH - 60, 16, GRAY);
}