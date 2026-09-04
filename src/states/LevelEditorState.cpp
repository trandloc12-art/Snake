#include "states/LevelEditorState.h"
#include "Game.h"
#include "enums/GameState.h"
#include "core/Constants.h"
#include "raylib.h"
#include <string>
#include <iostream>

LevelEditorState::LevelEditorState(Game& game) : game(game) {}

void LevelEditorState::Init() {
    phase = EditorPhase::ASK_WIDTH;
    inputBuffer.clear();
    hasHead = false;
    lastSaveMessage.clear();
}

void LevelEditorState::Update() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game.ChangeState(GameState::MENU);
        return;
    }

    // ---- Bước nhập số (width/height) — dùng chung logic đọc số ----
    if (phase == EditorPhase::ASK_WIDTH || phase == EditorPhase::ASK_HEIGHT) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= '0' && key <= '9' && inputBuffer.length() < 3)
                inputBuffer += (char)key;
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !inputBuffer.empty())
            inputBuffer.pop_back();

        if (IsKeyPressed(KEY_ENTER) && !inputBuffer.empty()) {
            int value = std::stoi(inputBuffer);
            inputBuffer.clear();
            if (phase == EditorPhase::ASK_WIDTH) {
                pendingWidth = value;
                phase = EditorPhase::ASK_HEIGHT;
            } else {
                level.Init(pendingWidth, value);
                phase = EditorPhase::DRAWING;
            }
        }
        return;
    }

    // ---- Bước nhập tên file (chữ + số) ----
    if (phase == EditorPhase::ASK_FILENAME) {
        int key = GetCharPressed();
        while (key > 0) {
            // Cho phép chữ cái, số, dấu gạch dưới - tránh ký tự đặc biệt gây lỗi tên file
            bool valid = (key >= '0' && key <= '9') ||
                         (key >= 'a' && key <= 'z') ||
                         (key >= 'A' && key <= 'Z') ||
                         key == '_';
            if (valid && inputBuffer.length() < 30) {
                inputBuffer += (char)key;
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !inputBuffer.empty())
            inputBuffer.pop_back();

        if (IsKeyPressed(KEY_ENTER) && !inputBuffer.empty()) {
            std::string fullPath = std::string(LEVELS_DIRECTORY) +"/" + inputBuffer + ".txt";

            bool success = level.SaveToFile(fullPath);
            lastSaveMessage = success
                ? ("Da luu: " + inputBuffer + ".txt")
                : "LOI: Khong the luu file!";

            inputBuffer.clear();
            phase = EditorPhase::DRAWING;
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            // Hủy đặt tên, quay lại vẽ (không lưu)
            inputBuffer.clear();
            phase = EditorPhase::DRAWING;
        }
        return;
    }

    // ---- Bước DRAWING ----
    Vector2 mouse = GetMousePosition();
    int cellX = (int)(mouse.x / cellSize);
    int cellY = (int)(mouse.y / cellSize);

    if (IsKeyPressed(KEY_ZERO)) currentBrush = CellType::EMPTY;
    if (IsKeyPressed(KEY_ONE))  currentBrush = CellType::WALL;
    if (IsKeyPressed(KEY_H))    currentBrush = CellType::SNAKE_HEAD;
    if (IsKeyPressed(KEY_UP))    currentBrush = CellType::SNAKE_LINK_UP;
    if (IsKeyPressed(KEY_DOWN))  currentBrush = CellType::SNAKE_LINK_DOWN;
    if (IsKeyPressed(KEY_LEFT))  currentBrush = CellType::SNAKE_LINK_LEFT;
    if (IsKeyPressed(KEY_RIGHT)) currentBrush = CellType::SNAKE_LINK_RIGHT;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (currentBrush == CellType::SNAKE_HEAD && hasHead) {
            // đã có head, bỏ qua
        } else {
            if (currentBrush == CellType::SNAKE_HEAD) hasHead = true;
            level.SetCell(cellX, cellY, currentBrush);
        }
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        if (level.GetCell(cellX, cellY) == CellType::SNAKE_HEAD) hasHead = false;
        level.SetCell(cellX, cellY, CellType::EMPTY);
    }

    // Bấm S -> chuyển sang bước NHẬP TÊN FILE, không lưu ngay lập tức
    if (IsKeyPressed(KEY_S)) {
        phase = EditorPhase::ASK_FILENAME;
        inputBuffer.clear();
        lastSaveMessage.clear();
    }
}

void LevelEditorState::Draw() {
    if (phase == EditorPhase::ASK_WIDTH) {
        DrawText("Nhap chieu rong (width), Enter de xac nhan:", 20, 20, 20, BLACK);
        DrawText(inputBuffer.c_str(), 20, 50, 30, DARKBLUE);
        DrawText("ESC: Quay lai Menu", 20, 550, 16, GRAY);
        return;
    }
    if (phase == EditorPhase::ASK_HEIGHT) {
        DrawText("Nhap chieu cao (height), Enter de xac nhan:", 20, 20, 20, BLACK);
        DrawText(inputBuffer.c_str(), 20, 50, 30, DARKBLUE);
        DrawText("ESC: Quay lai Menu", 20, 550, 16, GRAY);
        return;
    }

    // Vẽ lưới màn chơi (luôn hiển thị, kể cả lúc đang nhập tên file, để không mất ngữ cảnh)
    for (int y = 0; y < level.GetHeight(); y++) {
        for (int x = 0; x < level.GetWidth(); x++) {
            Rectangle cell = { (float)(x * cellSize), (float)(y * cellSize),
                                (float)cellSize, (float)cellSize };
            Color c;
            switch (level.GetCell(x, y)) {
                case CellType::WALL: c = DARKGRAY; break;
                case CellType::SNAKE_HEAD: c = RED; break;
                case CellType::SNAKE_LINK_UP:
                case CellType::SNAKE_LINK_DOWN:
                case CellType::SNAKE_LINK_LEFT:
                case CellType::SNAKE_LINK_RIGHT: c = GREEN; break;
                default: c = RAYWHITE; break;
            }
            DrawRectangleRec(cell, c);
            DrawRectangleLinesEx(cell, 1, LIGHTGRAY);
        }
    }

    int uiY = level.GetHeight() * cellSize + 10;

    if (phase == EditorPhase::ASK_FILENAME) {
        // Vẽ 1 lớp phủ mờ để làm nổi bật hộp nhập tên file
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
        DrawText("Nhap ten file (khong dau, khong khoang trang):", 200, 250, 20, WHITE);
        DrawText((inputBuffer + "_").c_str(), 200, 280, 30, YELLOW); // dấu "_" giả làm con trỏ nhấp nháy
        DrawText("ENTER: Luu | ESC: Huy", 200, 320, 16, LIGHTGRAY);
        return;
    }

    DrawText("0:Empty 1:Wall H:Head Arrow:SnakeLink  S:Save  ESC:Menu", 10, uiY, 16, DARKGRAY);

    if (!lastSaveMessage.empty()) {
        Color msgColor = (lastSaveMessage.find("LOI") == 0) ? RED : DARKGREEN;
        DrawText(lastSaveMessage.c_str(), 10, uiY + 25, 16, msgColor);
    }
}