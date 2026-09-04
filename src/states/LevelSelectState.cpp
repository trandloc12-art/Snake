#include "states/LevelSelectState.h"
#include "Game.h"
#include "enums/GameState.h"
#include "core/Constants.h"
#include "raylib.h"
#include <filesystem>
#include <iostream>

LevelSelectState::LevelSelectState(Game& game) : game(game) {}

// LevelSelectState.cpp — Init(), bỏ GetApplicationDirectory()
void LevelSelectState::Init() {
    selectedIndex = 0;
    levelFiles.clear();

    std::cout << "Dang quet thu muc: " << LEVELS_DIRECTORY << "\n"; // THÊM DÒNG NÀY


    FilePathList files = LoadDirectoryFilesEx(LEVELS_DIRECTORY, ".txt", false);

    std::cout << "Tim thay " << files.count << " file\n"; // THÊM DÒNG NÀY

    for (unsigned int i = 0; i < files.count; i++) {
        std::cout << "  - " << files.paths[i] << "\n"; // THÊM DÒNG NÀY

        std::filesystem::path p(files.paths[i]);
        LevelEntry entry;
        entry.fullPath = files.paths[i];
        entry.fileName = p.filename().string();
        levelFiles.push_back(entry);
    }
    UnloadDirectoryFiles(files);
}

void LevelSelectState::Update() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game.ChangeState(GameState::MENU);
        return;
    }

    if (levelFiles.empty()) return; // chỉ còn ESC để thoát khi không có màn nào

    if (IsKeyPressed(KEY_DOWN)) selectedIndex = (selectedIndex + 1) % (int)levelFiles.size();
    if (IsKeyPressed(KEY_UP))   selectedIndex = (selectedIndex - 1 + (int)levelFiles.size()) % (int)levelFiles.size();

    if (IsKeyPressed(KEY_ENTER)) {
        game.SetSelectedLevelPath(levelFiles[selectedIndex].fullPath);
        game.ChangeState(GameState::PLAYING);
    }
}

void LevelSelectState::Draw() {
    DrawText("CHON MAN CHOI", 260, 60, 30, DARKBLUE);
    DrawText(TextFormat("Tim thay %d man choi", (int)levelFiles.size()), 260, 100, 18, GRAY);

    if (levelFiles.empty()) {
        DrawText("Khong tim thay man choi nao trong assets/levels/", 150, 200, 20, RED);
        DrawText("Hay tao man choi moi bang Level Editor truoc", 150, 230, 18, GRAY);
        DrawText("ESC: Quay lai Menu", 200, 550, 16, GRAY);
        return;
    }

    for (size_t i = 0; i < levelFiles.size(); i++) {
        Color color = ((int)i == selectedIndex) ? RED : BLACK;
        DrawText(levelFiles[i].fileName.c_str(), 250, 150 + (int)i * 30, 20, color);
    }

    DrawText("ENTER: Choi | ESC: Quay lai Menu", 200, 550, 16, GRAY);
}