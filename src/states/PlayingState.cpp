#include "states/PlayingState.h"
#include "Game.h"
#include "enums/GameState.h"
#include "enums/CellType.h"
#include "core/Constants.h"
#include <vector>
#include <iostream>

PlayingState::PlayingState(Game& game, const AssetManager& assets)
    : game(game), assets(assets), snakeRenderer(assets) {}

void PlayingState::Init() {
    bool loaded = level.LoadFromFile(game.GetSelectedLevelPath());
    if (!loaded) {
        std::cerr << "PlayingState::Init - Khong the load level: "
                  << game.GetSelectedLevelPath() << "\n";
        game.ChangeState(GameState::MENU);
        return;
    }

    snake.InitFromLevel(level);
    if (snake.GetSegments().empty()) {
        std::cerr << "PlayingState::Init - Level khong co SNAKE_HEAD hop le\n";
        game.ChangeState(GameState::MENU);
        return;
    }

    pendingDirection = snake.GetCurrentDirection();
    moveTimer = 0.0f;
    score = 0;
    SpawnFood();
}

bool PlayingState::IsWallAt(int x, int y) const {
    return level.GetCell(x, y) == CellType::WALL;
}

void PlayingState::SpawnFood() {
    std::vector<Vector2> emptyCells;
    for (int y = 0; y < level.GetHeight(); y++) {
        for (int x = 0; x < level.GetWidth(); x++) {
            if (level.GetCell(x, y) == CellType::EMPTY && !snake.OccupiesCell(x, y)) {
                emptyCells.push_back({ (float)x, (float)y });
            }
        }
    }
    if (emptyCells.empty()) return;
    int index = GetRandomValue(0, (int)emptyCells.size() - 1);
    foodPosition = emptyCells[index];
}

void PlayingState::Update() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game.ChangeState(GameState::MENU);
        return;
    }

    if (IsKeyPressed(KEY_UP))    pendingDirection = Direction::UP;
    if (IsKeyPressed(KEY_DOWN))  pendingDirection = Direction::DOWN;
    if (IsKeyPressed(KEY_LEFT))  pendingDirection = Direction::LEFT;
    if (IsKeyPressed(KEY_RIGHT)) pendingDirection = Direction::RIGHT;

    moveTimer += GetFrameTime();
    if (moveTimer < moveInterval) return;
    moveTimer = 0.0f;

    snake.SetDirection(pendingDirection);

    Vector2 head = snake.GetHeadPosition();
    Direction dir = snake.GetCurrentDirection();
    int nextX = (int)head.x + (dir == Direction::RIGHT ? 1 : dir == Direction::LEFT ? -1 : 0);
    int nextY = (int)head.y + (dir == Direction::DOWN  ? 1 : dir == Direction::UP   ? -1 : 0);

    bool ateFood = ((float)nextX == foodPosition.x && (float)nextY == foodPosition.y);

    snake.Move(ateFood);

    Vector2 newHead = snake.GetHeadPosition();

    if (IsWallAt((int)newHead.x, (int)newHead.y)) {
        game.SetLastScore(score);
        game.ChangeState(GameState::GAME_OVER);
        return;
    }

    const auto& segments = snake.GetSegments();
    for (size_t i = 1; i < segments.size(); i++) {
        if ((int)segments[i].x == (int)newHead.x && (int)segments[i].y == (int)newHead.y) {
            game.SetLastScore(score);
            game.ChangeState(GameState::GAME_OVER);
            return;
        }
    }

    if (ateFood) {
        score++;
        SpawnFood();
    }
}

// Vẽ 1 texture vừa khít vào ô lưới, không xoay - dùng chung cho tường và mồi.
// Giữ chung logic DrawTexturePro với SnakeRenderer để đảm bảo mọi sprite trong game
// đều co giãn/hiển thị theo cùng 1 quy tắc (source = kích thước gốc, dest = cellSize).
static void DrawTileTexture(const Texture2D& tex, int gridX, int gridY, int cellSize) {
    Rectangle source = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dest = { (float)(gridX * cellSize), (float)(gridY * cellSize),
                        (float)cellSize, (float)cellSize };
    Vector2 origin = { 0, 0 }; // không xoay -> origin góc trên-trái là đủ, không cần tâm ô
    DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE);
}

void PlayingState::DrawLevel() const {
    // Vẽ nền (background) trước, sau đó vẽ tường (wall) lên trên.
    const Texture2D& backgroundTex = assets.GetTexture("playing_background");
    DrawTexturePro(backgroundTex, { 0, 0, (float)backgroundTex.width, (float)backgroundTex.height }, { 0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT }, { 0, 0 }, 0.0f, WHITE);

    const Texture2D& wallTex = assets.GetTexture("wall");

    for (int y = 0; y < level.GetHeight(); y++) {
        for (int x = 0; x < level.GetWidth(); x++) {
            if (IsWallAt(x, y)) {
                DrawTileTexture(wallTex, x, y, cellSize);
            }
        }
    }
}

void PlayingState::Draw() {
    DrawLevel();

    // Vẽ mồi
    const Texture2D& foodTex = assets.GetTexture("food");
    DrawTileTexture(foodTex, (int)foodPosition.x, (int)foodPosition.y, cellSize);
    // Vẽ rắn
    snakeRenderer.Draw(snake, cellSize);

    DrawText(TextFormat("Diem: %d", score), 10, 10, 20, BLACK);
}