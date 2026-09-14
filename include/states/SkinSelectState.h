#pragma once
#include "raylib.h"

class Game;

/// Màn hình liệt kê các skin rắn hiện có, cho phép chọn bằng bàn phím hoặc chuột.
/// Dùng danh sách chữ đơn giản (giống MenuState bản đầu tiên) vì chưa có ảnh nút
/// riêng cho từng skin - có thể nâng cấp lên ảnh xem trước sau này.
class SkinSelectState {
public:
    explicit SkinSelectState(Game& game);

    void Init();
    void Update();
    void Draw();

private:
    Game& game;
    int selectedIndex = 0;
};