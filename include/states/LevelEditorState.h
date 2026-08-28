// include/states/LevelEditorState.h
#pragma once
#include <string>
#include "Snake/Level.h"
#include "Snake/CellType.h"

enum class EditorPhase {
    ASK_WIDTH,    // đang hỏi chiều rộng
    ASK_HEIGHT,   // đang hỏi chiều cao
    DRAWING       // đang vẽ màn
};

class LevelEditorState {
public:
    void Init();                 // gọi khi vào state này
    void Update();
    void Draw();

private:
    EditorPhase phase = EditorPhase::ASK_WIDTH;
    std::string inputBuffer;     // chuỗi số đang gõ dở
    int pendingWidth = 0;

    Level level;
    CellType currentBrush = CellType::WALL;
    int cellSize = 32;

    bool hasHead = false;        // đảm bảo chỉ có 1 HEAD trong màn
};