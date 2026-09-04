#pragma once

class Game;

/// Màn hình hiện điểm số sau khi thua, chờ người chơi bấm Enter để quay lại Menu.
class GameOverState {
public:
    explicit GameOverState(Game& game);

    void Init();
    void Update();
    void Draw();

private:
    Game& game;
};