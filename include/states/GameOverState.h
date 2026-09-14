#pragma once
#include "raylib.h"
#include "ui/Button.h"
#include <array>

class Game;

class GameOverState {
public:
    explicit GameOverState(Game& game);

    void Init();
    void Update();
    void Draw();

private:
    Game& game;
    int selectedOption = 0; // 0 = Choi lai, 1 = Ve Menu

    std::array<Button, 2> buttons;
    const Texture2D* titleTexture = nullptr;
    Vector2 titlePosition{};

    const Texture2D* deadSnakeTexture = nullptr;
    Vector2 deadSnakePosition{};
};