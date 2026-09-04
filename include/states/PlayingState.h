#pragma once
#include "raylib.h"
#include "Snake/Snake.h"
#include "Snake/SnakeRenderer.h"
#include "Snake/Level.h"
#include "core/AssetManager.h"
#include "enums/Direction.h"

class Game;

/// State chơi chính: chứa Level (bản đồ), Snake (dữ liệu), SnakeRenderer (vẽ),
/// xử lý input, di chuyển theo nhịp thời gian cố định, ăn mồi, va chạm.
class PlayingState {
public:
    PlayingState(Game& game, const AssetManager& assets);

    void Init();   // load Level từ đường dẫn Game.GetSelectedLevelPath(), dựng Snake
    void Update();
    void Draw();

private:
    void SpawnFood();          // đặt mồi mới ở 1 ô trống ngẫu nhiên
    bool IsWallAt(int x, int y) const;

    Game& game;
    SnakeRenderer snakeRenderer; // khởi tạo với assets, không tự sở hữu texture

    Level level;
    Snake snake;

    Direction pendingDirection = Direction::RIGHT; // hướng người chơi vừa bấm, áp dụng ở bước di chuyển kế tiếp
    float moveTimer = 0.0f;
    float moveInterval = 0.15f;
    int cellSize = 32;

    Vector2 foodPosition = { -1, -1 };
    int score = 0;
};