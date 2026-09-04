#pragma once
#include "raylib.h"
#include "enums/GameState.h"
#include "core/AssetManager.h"
#include "states/MenuState.h"
#include "states/LevelSelectState.h"
#include "states/LevelEditorState.h"
#include "states/PlayingState.h"
#include "states/GameOverState.h"
#include <string>

/// Class trung tâm: quản lý vòng đời cửa sổ, state hiện tại, và điều phối
/// chuyển đổi giữa các state. Sở hữu DUY NHẤT 1 AssetManager, các state khác
/// chỉ nhận tham chiếu, không tự load asset riêng.
class Game {
public:
    Game();
    ~Game();

    /// Chạy vòng lặp chính — gọi 1 lần duy nhất từ main().
    void Run();
    
    /// Được các state gọi để yêu cầu chuyển sang state khác.
    void ChangeState(GameState newState);

    /// Dữ liệu chia sẻ giữa các state (ví dụ: LevelSelectState chọn file,
    /// PlayingState cần biết để load đúng màn).
    void SetSelectedLevelPath(const std::string& path) { selectedLevelPath = path; }
    const std::string& GetSelectedLevelPath() const { return selectedLevelPath; }

    void SetLastScore(int score) { lastScore = score; }
    int GetLastScore() const { return lastScore; }

    void RequestQuit() { quitRequested = true; }// Các state gọi hàm này để yêu cầu thoát game (ví dụ: MenuState khi bấm ESC).

private:
    void Update();
    void Draw();

    // QUAN TRỌNG: assets phải khai báo TRƯỚC các state trong class, vì thứ tự
    // khởi tạo member trong C++ theo đúng thứ tự KHAI BÁO (không theo initializer
    // list) — các state cần assets đã sẵn sàng lúc chúng được tạo.
    AssetManager assets;

    GameState currentState;
    GameState previousState; // dùng để phát hiện lúc VỪA chuyển state, gọi Init() đúng 1 lần

    MenuState menuState;
    LevelSelectState levelSelectState;
    LevelEditorState levelEditorState;
    PlayingState playingState;
    GameOverState gameOverState;

    bool quitRequested = false;// true nếu state hiện tại muốn thoát game (ví dụ: MenuState khi bấm ESC)

    std::string selectedLevelPath;
    int lastScore = 0;
};