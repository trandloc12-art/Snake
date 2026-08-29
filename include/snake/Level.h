#pragma once
#include <vector>
#include <string>
#include "enums/CellType.h"

class Level {
public:
    void Init(int width, int height);
    bool LoadFromFile(const std::string& path);
    bool SaveToFile(const std::string& path) const;

    CellType GetCell(int x, int y) const;
    void SetCell(int x, int y, CellType type);

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

private:
    std::vector<std::vector<CellType>> grid;
    int width = 0;
    int height = 0;
};