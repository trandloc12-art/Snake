#pragma once

class Game; // forward declaration — chỉ cần con trỏ/tham chiếu, giảm phụ thuộc include

/// Màn hình chính: chọn Chơi, Sửa màn (Editor), hoặc Thoát.
class MenuState {
public:
    explicit MenuState(Game& game);

    void Init();
    void Update();
    void Draw();

private:
    Game& game; // dùng để gọi game.ChangeState(...) khi người chơi chọn 1 mục
    int selectedOption = 0; // 0 = Play, 1 = Editor, 2 = Quit
};