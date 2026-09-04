#pragma once
#include <vector>
#include <string>

class Game;

class LevelSelectState {
public:
    explicit LevelSelectState(Game& game);

    void Init();
    void Update();
    void Draw();

private:
    Game& game;

    struct LevelEntry {
        std::string fullPath;  // dùng để LoadFromFile()
        std::string fileName;  // chỉ tên file, dùng để HIỂN THỊ (gọn hơn)
    };

    std::vector<LevelEntry> levelFiles;
    int selectedIndex = 0;
};