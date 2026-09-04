#pragma once
#include <string>
#include "Snake/Level.h"
#include "enums/CellType.h"

class Game;

enum class EditorPhase {
    ASK_WIDTH,
    ASK_HEIGHT,
    DRAWING,
    ASK_FILENAME   // MỚI: hỏi tên file trước khi thực sự ghi ra đĩa
};

class LevelEditorState {
public:
    explicit LevelEditorState(Game& game);

    void Init();
    void Update();
    void Draw();

private:
    Game& game;

    EditorPhase phase = EditorPhase::ASK_WIDTH;
    std::string inputBuffer;
    int pendingWidth = 0;

    Level level;
    CellType currentBrush = CellType::WALL;
    int cellSize = 32;
    bool hasHead = false;

    std::string lastSaveMessage; // hiển thị "Da luu thanh cong" / lỗi, để người dùng thấy phản hồi
};