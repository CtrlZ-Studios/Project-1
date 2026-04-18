#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include "raylib.h"
#include <vector>

const int WORLD_WIDTH = 2000;
const int WORLD_HEIGHT = 450;
const int TILE_SIZE = 16;

class MapManager {
public:
    MapManager();
    ~MapManager();

    void Draw();
    Rectangle GetFloorBounds() const;

private:
    Texture2D tileset;
    bool tilesetLoaded;
    Rectangle floorRect;
};

#endif // MAP_MANAGER_H
