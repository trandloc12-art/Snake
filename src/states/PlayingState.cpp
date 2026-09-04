#include "states/PlayingState.h"
#include "Game.h"
#include "enums/GameState.h"
#include "enums/CellType.h"
#include "core/Constants.h"
#include <vector>
#include <iostream>


PlayingState::PlayingState(Game& game, const AssetManager& assets)
    : game(game), snakeRenderer(assets) {}

void PlayingState::Init() {
    bool loaded = level.LoadFromFile(game.GetSelectedLevelPath());

    if (!loaded) {
        // Không load được -> không nên chơi với dữ liệu rác, quay lại Menu
        // và báo lỗi rõ ràng thay vì im lặng tiếp tục.
        std::cerr << "PlayingState::Init - Khong the load level: "
                  << game.GetSelectedLevelPath() << "\n";
        game.ChangeState(GameState::MENU);
        return;
    }

    snake.InitFromLevel(level);

    // Kiểm tra thêm: nếu Level không có HEAD, snake sẽ rỗng -> cũng nên chặn sớm
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
    // Thu thập toàn bộ ô trống, không có tường và không có thân rắn
    std::vector<Vector2> emptyCells;
    for (int y = 0; y < level.GetHeight(); y++) {
        for (int x = 0; x < level.GetWidth(); x++) {
            if (level.GetCell(x, y) == CellType::EMPTY && !snake.OccupiesCell(x, y)) {
                emptyCells.push_back({ (float)x, (float)y });
            }
        }
    }

    if (emptyCells.empty()) return; // hết chỗ trống -> rắn đã chiếm toàn bộ màn (thắng)

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

    // Kiểm tra va chạm SAU khi di chuyển
    if (IsWallAt((int)newHead.x, (int)newHead.y)) {
        game.SetLastScore(score);
        game.ChangeState(GameState::GAME_OVER);
        return;
    }

    // Kiểm tra tự cắn: bỏ qua chính đầu (index 0), kiểm tra từ đốt thứ 2 trở đi
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

void PlayingState::Draw() {
    // Vẽ tường (tạm dùng hình chữ nhật màu — có thể đổi sang texture "wall" sau)
    for (int y = 0; y < level.GetHeight(); y++) {
        for (int x = 0; x < level.GetWidth(); x++) {
            if (IsWallAt(x, y)) {
                DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, DARKGRAY);
            }
        }
    }

    // Vẽ mồi
    DrawRectangle((int)(foodPosition.x * cellSize), (int)(foodPosition.y * cellSize),
                  cellSize, cellSize, RED);

    // Vẽ rắn — giao toàn bộ việc vẽ cho SnakeRenderer
    snakeRenderer.Draw(snake, cellSize);

    DrawText(TextFormat("Diem: %d", score), 10, 10, 20, BLACK);
}