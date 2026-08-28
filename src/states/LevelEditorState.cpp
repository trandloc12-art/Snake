// src/states/LevelEditorState.cpp
#include "states/LevelEditorState.h"
#include "raylib.h"
#include <string>

/// Reset về bước hỏi width, xóa input cũ và cờ hasHead — gọi khi vừa vào state.
void LevelEditorState::Init() {
    phase = EditorPhase::ASK_WIDTH;
    inputBuffer.clear();
    hasHead = false;
}

/// Xử lý input mỗi frame: ASK_WIDTH/ASK_HEIGHT nhận số qua bàn phím,
/// DRAWING nhận brush + chuột để vẽ ô, và phím S để lưu file.
void LevelEditorState::Update() {
    // --- Bước nhập số (dùng chung cho cả width và height) ---
    if (phase == EditorPhase::ASK_WIDTH || phase == EditorPhase::ASK_HEIGHT) {
        // Đọc mọi ký tự gõ trong frame này (có thể nhiều ký tự nếu gõ nhanh)
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= '0' && key <= '9' && inputBuffer.length() < 3)
                inputBuffer += (char)key;
            key = GetCharPressed();
        }

        // Xóa ký tự cuối
        if (IsKeyPressed(KEY_BACKSPACE) && !inputBuffer.empty())
            inputBuffer.pop_back();

        // Xác nhận giá trị, chuyển bước tiếp theo
        if (IsKeyPressed(KEY_ENTER) && !inputBuffer.empty()) {
            int value = std::stoi(inputBuffer);
            inputBuffer.clear();

            if (phase == EditorPhase::ASK_WIDTH) {
                pendingWidth = value;
                phase = EditorPhase::ASK_HEIGHT;
            } else {
                level.Init(pendingWidth, value); // đủ width+height -> tạo lưới trống
                phase = EditorPhase::DRAWING;
            }
        }
        return; // chưa vẽ thì không xử lý chuột/brush bên dưới
    }

    // --- Bước vẽ màn ---
    Vector2 mouse = GetMousePosition();
    int cellX = (int)(mouse.x / cellSize); // đổi pixel -> tọa độ ô
    int cellY = (int)(mouse.y / cellSize);

    // Đổi brush bằng phím tắt
    if (IsKeyPressed(KEY_ZERO)) currentBrush = CellType::EMPTY;
    if (IsKeyPressed(KEY_ONE))  currentBrush = CellType::WALL;
    if (IsKeyPressed(KEY_H))    currentBrush = CellType::SNAKE_HEAD;
    if (IsKeyPressed(KEY_UP))    currentBrush = CellType::SNAKE_UP;
    if (IsKeyPressed(KEY_DOWN))  currentBrush = CellType::SNAKE_DOWN;
    if (IsKeyPressed(KEY_LEFT))  currentBrush = CellType::SNAKE_LEFT;
    if (IsKeyPressed(KEY_RIGHT)) currentBrush = CellType::SNAKE_RIGHT;

    // Giữ chuột trái để "sơn" ô liên tục; chặn đặt HEAD thứ 2
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (currentBrush == CellType::SNAKE_HEAD && hasHead) {
            // đã có head, bỏ qua click này
        } else {
            if (currentBrush == CellType::SNAKE_HEAD) hasHead = true;
            level.SetCell(cellX, cellY, currentBrush);
        }
    }

    // Click phải 1 lần để xóa ô (đặt về EMPTY)
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        if (level.GetCell(cellX, cellY) == CellType::SNAKE_HEAD) hasHead = false;
        level.SetCell(cellX, cellY, CellType::EMPTY);
    }

    // Lưu màn ra file
    if (IsKeyPressed(KEY_S)) {
        level.SaveToFile("assets/levels/level1.txt");
    }
}

/// Vẽ giao diện mỗi frame: ô nhập số ở 2 bước đầu, lưới màn chơi + hướng dẫn ở bước vẽ.
void LevelEditorState::Draw() {
    // Màn hình nhập width
    if (phase == EditorPhase::ASK_WIDTH) {
        DrawText("Nhap chieu rong (width), Enter de xac nhan:", 20, 20, 20, BLACK);
        DrawText(inputBuffer.c_str(), 20, 50, 30, DARKBLUE);
        return;
    }
    // Màn hình nhập height
    if (phase == EditorPhase::ASK_HEIGHT) {
        DrawText("Nhap chieu cao (height), Enter de xac nhan:", 20, 20, 20, BLACK);
        DrawText(inputBuffer.c_str(), 20, 50, 30, DARKBLUE);
        return;
    }

    // Vẽ từng ô trong lưới theo loại CellType, kèm viền để thấy rõ ranh giới ô
    for (int y = 0; y < level.GetHeight(); y++) {
        for (int x = 0; x < level.GetWidth(); x++) {
            Rectangle cell = { (float)(x * cellSize), (float)(y * cellSize),
                                (float)cellSize, (float)cellSize };
            Color c;
            switch (level.GetCell(x, y)) {
                case CellType::WALL: c = DARKGRAY; break;
                case CellType::SNAKE_HEAD: c = RED; break;
                case CellType::SNAKE_UP:
                case CellType::SNAKE_DOWN:
                case CellType::SNAKE_LEFT:
                case CellType::SNAKE_RIGHT: c = GREEN; break;
                default: c = RAYWHITE; break; // EMPTY
            }
            DrawRectangleRec(cell, c);
            DrawRectangleLinesEx(cell, 1, LIGHTGRAY);
        }
    }

    // Thanh hướng dẫn phím tắt, đặt ngay dưới lưới
    int uiY = level.GetHeight() * cellSize + 10;
    DrawText("0:Empty 1:Wall H:Head Arrow:SnakeDir  S:Save  LeftClick:Ve  RightClick:Xoa",
              10, uiY, 16, DARKGRAY);
}