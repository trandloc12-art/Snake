// src/Snake/Level.cpp
#include "Snake/snake/Level.h"
#include <fstream>
#include <sstream>
#include <unordered_map>

// Chuyển CellType -> chuỗi để ghi file
static std::string CellToString(CellType type) {
    switch (type) {
        case CellType::EMPTY: return "0";
        case CellType::WALL: return "WALL";
        case CellType::SNAKE_HEAD: return "HEAD";
        case CellType::SNAKE_UP: return "UP";
        case CellType::SNAKE_DOWN: return "DOWN";
        case CellType::SNAKE_LEFT: return "LEFT";
        case CellType::SNAKE_RIGHT: return "RIGHT";
    }
    return "0";
}

// Chuyển chuỗi -> CellType khi đọc file
static CellType StringToCell(const std::string& token) {
    static const std::unordered_map<std::string, CellType> map = {
        {"0", CellType::EMPTY},
        {"WALL", CellType::WALL},
        {"HEAD", CellType::SNAKE_HEAD},
        {"UP", CellType::SNAKE_UP},
        {"DOWN", CellType::SNAKE_DOWN},
        {"LEFT", CellType::SNAKE_LEFT},
        {"RIGHT", CellType::SNAKE_RIGHT},
    };
    auto it = map.find(token);
    return it != map.end() ? it->second : CellType::EMPTY;
}

void Level::Init(int w, int h) {
    width = w;
    height = h;
    grid.assign(height, std::vector<CellType>(width, CellType::EMPTY));
}

bool Level::LoadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    grid.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::vector<CellType> row;
        std::istringstream iss(line);
        std::string token;
        while (iss >> token) {
            row.push_back(StringToCell(token));
        }
        if (!row.empty()) grid.push_back(row);
    }

    height = (int)grid.size();
    width = height > 0 ? (int)grid[0].size() : 0;
    return true;
}

bool Level::SaveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file.is_open()) return false;

    for (const auto& row : grid) {
        for (size_t x = 0; x < row.size(); x++) {
            file << CellToString(row[x]);
            if (x + 1 < row.size()) file << " ";
        }
        file << "\n";
    }
    return true;
}

CellType Level::GetCell(int x, int y) const {
    if (y < 0 || y >= height || x < 0 || x >= width) return CellType::WALL;
    return grid[y][x];
}

void Level::SetCell(int x, int y, CellType type) {
    if (y < 0 || y >= height || x < 0 || x >= width) return;
    grid[y][x] = type;
}