#include "Game.h"
#include <iostream>
#include <filesystem>

int main() {
        std::cout << "Working directory hien tai: " << std::filesystem::current_path() << "\n";
    Game game;
    game.Run();
    return 0;
}