#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include "raylib.h"
#include <vector>

const int MAP_ROWS = 15;
const int MAP_COLS = 79;
const int MAP2_COLS = 293;
const int MAX_MAP_COLS = 293;
const int TILE_SIZE = 16;
const int WORLD_HEIGHT = MAP_ROWS * TILE_SIZE;

struct EnemySpawn {
    int type; 
    Vector2 position;
};

struct InteractionResult {
    int tileID;
    int row;
    int col;
};

class MapManager {
public:
    MapManager();
    ~MapManager();

    void DrawBackground();
    void DrawTiles();
    void DrawForeground();
    void DrawTile(int tileID, Vector2 position);
    void LoadLevel(int levelIndex);
    InteractionResult InteractWithMap(Rectangle hitbox, int interactionType);
    bool CheckCollision(Rectangle hitbox) const;
    void CullOffscreen(float leftEdgeX);
    
    Vector2 GetSpawnPosition() const { return spawnPosition; }
    const std::vector<EnemySpawn>& GetEnemySpawns() const { return enemySpawns; }
    int GetCurrentLevel() const { return currentLevel; }
    int GetMapCols() const { return currentCols; }
    int GetWorldWidth() const { return currentCols * TILE_SIZE; }

    int GetTile(int r, int c) const {
        if (r >= 0 && r < MAP_ROWS && c >= 0 && c < currentCols) return mapData[r][c];
        return 0;
    }

    void SetTile(int r, int c, int tileID) {
        if (r >= 0 && r < MAP_ROWS && c >= 0 && c < currentCols) mapData[r][c] = tileID;
    }

private:
    Texture2D tileset;
    bool tilesetLoaded;
    int currentLevel;
    int currentCols;
    std::vector<EnemySpawn> enemySpawns;
    int mapData[15][MAX_MAP_COLS];
    Vector2 spawnPosition;
};

#endif // MAP_MANAGER_H
