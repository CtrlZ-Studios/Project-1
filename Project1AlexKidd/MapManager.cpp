#include "MapManager.h"
#include "raylib.h"
#include <iostream>

MapManager::MapManager() : tilesetLoaded(false) {
    tileset = LoadTexture("Sprites/tileset.png");
    if (tileset.id != 0) {
        tilesetLoaded = true;
    }
}

MapManager::~MapManager() {
    if (tilesetLoaded) {
        UnloadTexture(tileset);
    }
}

void MapManager::Draw() {
    Rectangle skySource = { 16, 16, 16, 16 }; // Row 2, Col 2 (1-indexed)

    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            Vector2 pos = { (float)c * TILE_SIZE, (float)r * TILE_SIZE };
            
            // Draw Sky as base layer
            if (tilesetLoaded) {
                DrawTextureRec(tileset, skySource, pos, WHITE);
            } else {
                DrawRectangleV(pos, { (float)TILE_SIZE, (float)TILE_SIZE }, SKYBLUE);
            }

            // Draw specific tiles
            if (mapData[r][c] != 0) {
                if (tilesetLoaded) {
                    // THIS line right here was missing!
                    Rectangle source = { 0, 0, 16, 16 }; 
                    
                    switch (mapData[r][c]) {
                        case 1: source = { 16, 0, 16, 16 }; break;  // Solid (2nd tile, 1st row)
                        case 2: source = {0, 48, 16, 16}; break; // Destructible block (1st tile, 4th row)
                        case 3: source = {16, 48, 16, 16}; break; // Collectible block (2nd tile, 4th row)
                        case 4: source = {32, 16, 16, 16}; break; // Left wall (2nd tile, 2nd row)
                        case 5: source = {0, 16, 16, 16}; break; // Right wall (1st tile, 2nd row)
                    }
                    DrawTextureRec(tileset, source, pos, WHITE);
                } else {
                    Color color = BLACK;
                    switch (mapData[r][c]) {
                        case 1: color = BROWN; break;
                        case 2: color = ORANGE; break;
                        case 3: color = GOLD; break;
                    }
                    DrawRectangleV(pos, { (float)TILE_SIZE, (float)TILE_SIZE }, color);
                }
            }
        }
    }
}

void MapManager::InteractWithMap(Rectangle hitbox, int interactionType) {
    // Determine which tiles the hitbox overlaps
    int startCol = (int)(hitbox.x / TILE_SIZE);
    int endCol = (int)((hitbox.x + hitbox.width) / TILE_SIZE);
    int startRow = (int)(hitbox.y / TILE_SIZE);
    int endRow = (int)((hitbox.y + hitbox.height) / TILE_SIZE);

    // Clamp to map boundaries
    if (startCol < 0) startCol = 0;
    if (endCol >= MAP_COLS) endCol = MAP_COLS - 1;
    if (startRow < 0) startRow = 0;
    if (endRow >= MAP_ROWS) endRow = MAP_ROWS - 1;

    for (int r = startRow; r <= endRow; r++) {
        for (int c = startCol; c <= endCol; c++) {
            Rectangle tileRect = { (float)c * TILE_SIZE, (float)r * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
            
            if (CheckCollisionRecs(hitbox, tileRect)) {
                if (interactionType == 2 && mapData[r][c] == 2) {
                    // Attack destroys destructible blocks
                    mapData[r][c] = 0;
                } else if (interactionType == 3 && mapData[r][c] == 3) {
                    // Body collects items
                    mapData[r][c] = 0;
                    std::cout << "Collectible picked up at [" << r << "," << c << "]" << std::endl;
                }
            }
        }
    }
}

bool MapManager::CheckCollision(Rectangle hitbox) const {
    int startCol = (int)(hitbox.x / TILE_SIZE);
    int endCol = (int)((hitbox.x + hitbox.width) / TILE_SIZE);
    int startRow = (int)(hitbox.y / TILE_SIZE);
    int endRow = (int)((hitbox.y + hitbox.height) / TILE_SIZE);

    // Clamp to map boundaries
    if (startCol < 0) startCol = 0;
    if (endCol >= MAP_COLS) endCol = MAP_COLS - 1;
    if (startRow < 0) startRow = 0;
    if (endRow >= MAP_ROWS) endRow = MAP_ROWS - 1;

    for (int r = startRow; r <= endRow; r++) {
        for (int c = startCol; c <= endCol; c++) {
            if (mapData[r][c] == 1 || mapData[r][c] == 2 || mapData[r][c] == 4 || mapData[r][c] == 5) {
                Rectangle tileRect = { (float)c * TILE_SIZE, (float)r * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
                if (CheckCollisionRecs(hitbox, tileRect)) {
                    return true;
                }
            }
        }
    }
    return false;
}
