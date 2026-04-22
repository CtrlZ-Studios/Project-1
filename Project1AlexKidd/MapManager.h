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

    // Shop System Constants
    const float doorOffsetX = 31.0f; // 1.9375 blocks right
    const float doorOffsetY = -12.0f; // 1 block up
    const float oneUpOffsetX = 8.0f; // half block right
    const float oneUpOffsetY = -8.0f; // half block up
    const float exitOffsetX = 4.0f;
    const float exitOffsetY = 4.0f;

    void DrawBackground();
    void DrawTiles(bool shop1UpBought = false);
    void DrawForeground();
    void DrawTile(int tileID, Vector2 position, bool shop1UpBought = false);
    void LoadLevel(int levelIndex, bool returningFromShop = false);
    InteractionResult InteractWithMap(Rectangle hitbox, int interactionType);
    bool CheckCollision(Rectangle hitbox) const;
    void CullOffscreen(float leftEdgeX);
    
    Vector2 GetSafeRespawnPosition(float leftCameraX, const std::vector<class Enemy*>& enemies);

    Vector2 GetSpawnPosition() const { return spawnPosition; }
    const std::vector<EnemySpawn>& GetEnemySpawns() const { return enemySpawns; }
    int GetCurrentLevel() const { return currentLevel; }
    int GetMapCols() const { return currentCols; }
    int GetMapRows() const { return currentRows; }
    int GetWorldWidth() const { return currentCols * TILE_SIZE; }

    int GetTile(int r, int c) const {
        if (r >= 0 && r < currentRows && c >= 0 && c < currentCols) return mapData[r][c];
        return 0;
    }

    void SetTile(int r, int c, int tileID) {
        if (r >= 0 && r < currentRows && c >= 0 && c < currentCols) mapData[r][c] = tileID;
    }

private:
    Texture2D tileset;
    Texture2D shopTex, doorTex, shopInteriorTex, oneUpTex, exitTex;
    bool tilesetLoaded;
    int currentLevel;
    int currentCols;
    int currentRows;
    std::vector<EnemySpawn> enemySpawns;
    int mapData[15][MAX_MAP_COLS];
    Vector2 spawnPosition;
};

#endif // MAP_MANAGER_H
