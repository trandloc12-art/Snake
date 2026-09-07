#pragma once
#include "raylib.h"
#include "Snake/Snake.h"
#include "Snake/SnakeRenderer.h"
#include "Snake/Level.h"
#include "core/AssetManager.h"
#include "enums/Direction.h"

class Game;
    

class PlayingState {
public:
    PlayingState(Game& game, const AssetManager& assets);

    void Init();
    void Update();
    void Draw();

private:
    void SpawnFood();
    bool IsWallAt(int x, int y) const;
    void DrawLevel() const; // MỚI: tách riêng việc vẽ tường ra 1 hàm cho gọn Draw()

    Game& game;
    const AssetManager& assets; // MỚI: giữ tham chiếu để tự lấy texture wall/food
    SnakeRenderer snakeRenderer;

    Level level;
    Snake snake;

    Direction pendingDirection = Direction::RIGHT;
    float moveTimer = 0.0f;
    float moveInterval = 0.15f;
    int cellSize = 32;

    Vector2 foodPosition = { -1, -1 };
    int score = 0;
    // MỚI: đếm ngược chuẩn bị trước khi chơi
    float countdownTimer = 3.0f;
    bool isCountingDown = true;
};