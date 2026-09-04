#include "Game.h"
#include "core/Constants.h"

Game::Game()
    : currentState(GameState::MENU),
      previousState(GameState::MENU),
      menuState(*this),
      levelSelectState(*this),
      levelEditorState(*this),
      playingState(*this, assets),
      gameOverState(*this),
      selectedLevelPath(DEFAULT_LEVEL_FILE)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetExitKey(KEY_NULL); // tắt ESC mặc định của raylib -> tự xử lý logic ESC theo ý mình
    SetTargetFPS(TARGET_FPS);
    assets.LoadAll();
}

Game::~Game() {
    CloseWindow();
}

void Game::ChangeState(GameState newState) {
    currentState = newState;
    // previousState CHƯA đổi ở đây — Update() sẽ so sánh currentState với
    // previousState để biết "vừa chuyển state" và gọi Init() đúng 1 lần,
    // rồi mới cập nhật previousState = currentState.
}

void Game::Run() {
    while (!WindowShouldClose()) {
        Update();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        Draw();
        EndDrawing();
    }
}

void Game::Update() {
    bool justEntered = (currentState != previousState);

    switch (currentState) {
        case GameState::MENU:
            if (justEntered) menuState.Init();
            menuState.Update();
            break;
        case GameState::LEVEL_SELECT:
            if (justEntered) levelSelectState.Init();
            levelSelectState.Update();
            break;
        case GameState::LEVEL_EDITOR:
            if (justEntered) levelEditorState.Init();
            levelEditorState.Update();
            break;
        case GameState::PLAYING:
            if (justEntered) playingState.Init();
            playingState.Update();
            break;
        case GameState::GAME_OVER:
            if (justEntered) gameOverState.Init();
            gameOverState.Update();
            break;
    }

    previousState = currentState;
}

void Game::Draw() {
    switch (currentState) {
        case GameState::MENU:         menuState.Draw(); break;
        case GameState::LEVEL_SELECT: levelSelectState.Draw(); break;
        case GameState::LEVEL_EDITOR: levelEditorState.Draw(); break;
        case GameState::PLAYING:      playingState.Draw(); break;
        case GameState::GAME_OVER:    gameOverState.Draw(); break;
    }
}