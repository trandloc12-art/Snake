// include/states/LevelEditorState.h
#pragma once
#include <string>
#include "Snake/Level.h"
#include "enums/CellType.h"

/// Các bước con trong quy trình tạo màn chơi.
enum class EditorPhase {
    ASK_WIDTH,   ///< Đang chờ nhập chiều rộng
    ASK_HEIGHT,  ///< Đang chờ nhập chiều cao
    DRAWING      ///< Đang vẽ màn chơi
};

/// State cho phép tạo/chỉnh sửa màn chơi bằng chuột + phím tắt, lưu ra file text.
class LevelEditorState {
public:
    void Init();    ///< Reset state về trạng thái ban đầu, gọi 1 lần khi vào state
    void Update();  ///< Xử lý input mỗi frame (nhập số, vẽ ô, lưu file)
    void Draw();    ///< Vẽ giao diện mỗi frame (lưới, brush hiện tại, hướng dẫn)

private:
    EditorPhase phase = EditorPhase::ASK_WIDTH; ///< Bước hiện tại trong quy trình
    std::string inputBuffer;                    ///< Chuỗi số đang gõ dở (width/height)
    int pendingWidth = 0;                       ///< Width đã nhập, giữ tạm chờ nhập height

    Level level;                                ///< Dữ liệu màn chơi đang chỉnh sửa
    CellType currentBrush = CellType::WALL;      ///< Loại ô đang chọn để vẽ
    int cellSize = 32;                          ///< Kích thước 1 ô (pixel) khi vẽ lên màn hình

    bool hasHead = false;                       ///< Đảm bảo màn chỉ có đúng 1 SNAKE_HEAD
};