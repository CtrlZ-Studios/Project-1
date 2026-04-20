#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include "raylib.h"
#include <vector>

const int MAP_ROWS = 12;
const int MAP_COLS = 16;
const int TILE_SIZE = 16;
const int WORLD_WIDTH = MAP_COLS * TILE_SIZE;
const int WORLD_HEIGHT = MAP_ROWS * TILE_SIZE;

class MapManager {
public:
    MapManager();
    ~MapManager();

    void Draw();
    void InteractWithMap(Rectangle hitbox, int interactionType);
    bool CheckCollision(Rectangle hitbox) const;
    Vector2 GetSpawnPosition() const { return spawnPosition; }

private:
    Texture2D tileset;
    bool tilesetLoaded;
    // Assuming MAP_ROWS is 12 and MAP_COLS is 16
    int mapData[12][16] = {
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
        {4,0,0,0,0,0,0,0,0,0,0,1,0,1,0,5},
        {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
        {4,0,0,0,0,0,0,0,0,0,0,0,2,2,0,5},
        {4,0,0,0,0,0,0,0,0,0,0,1,1,1,0,5},
        {4,0,0,3,0,0,0,0,0,0,3,0,0,0,0,5},
        {4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5}
    };
    Vector2 spawnPosition;
};

#endif // MAP_MANAGER_H
