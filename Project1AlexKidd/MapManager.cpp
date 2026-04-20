#include "MapManager.h"
#include "raylib.h"
#include <iostream>
#include "PlayerManager.h"

MapManager::MapManager() : tilesetLoaded(false), spawnPosition({100, 150}) {
    // Scan for spawn marker (-1)
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            if (mapData[r][c] == -1) {
                // Align feet to the bottom of the tile
                spawnPosition = {
                    (float)c * TILE_SIZE + TILE_SIZE / 2.0f,
                    (float)r * TILE_SIZE + TILE_SIZE - PlayerManager::frameHeight
                };
                mapData[r][c] = 0; // Clear the marker
            }
        }
    }

    tileset = LoadTexture("Sprites/tileset.png");
    if (tileset.id != 0) tilesetLoaded = true;
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
                        case 1: source = { 16, 0, 16, 16 }; break;  // Grass block (2nd tile, 1st row)
                        case 2: source = {0, 48, 16, 16}; break; // Destructible block (1st tile, 4th row)
                        case 3: source = {16, 48, 16, 16}; break; // Collectible block (2nd tile, 4th row)
                        case 4: source = {32, 16, 16, 16}; break; // Left wall (2nd tile, 2nd row)
                        case 5: source = {0, 16, 16, 16}; break; // Right wall (1st tile, 2nd row)
                        case 6: source = {0, 0, 16, 16}; break; // Left corner grassy (1st tile, 1st row)
                        case 7: source = {32, 0, 16, 16}; break; // Right corner grassy (3rd tile, 1st row)
                        case 8: source = {0, 32, 16, 16}; break; // Left corner (1st tile, 3rd row)
                        case 9: source = {32, 32, 16, 16}; break; // Right corner (3rd tile, 3rd row)
                        case 10: source = {16, 32, 16, 16}; break; // Dirt (2nd tile, 3rd row)
                        case 11: source = {16, 16, 16, 16}; break; // Barrier block (not drawn, but solid)
                        case 12: source = {32, 128, 16, 16}; break; // Grass_2_top
                        case 13: source = {0, 144, 16, 16}; break; // Grass_2_block
                        case 14: source = {0, 128, 16, 16}; break; // Red Ball
                        case 15: source = {16, 128, 16, 16}; break; // Blue Ball

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

bool MapManager::InteractWithMap(Rectangle hitbox, int interactionType) {
    bool hitOccurred = false;
    
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

    if (interactionType == 2) {
        // Attack logic: Destroy only the block with the largest overlap area
        int bestR = -1;
        int bestC = -1;
        float maxArea = 0.0f;

        for (int r = startRow; r <= endRow; r++) {
            for (int c = startCol; c <= endCol; c++) {
                if (mapData[r][c] == 2 || mapData[r][c] == 15) {
                    Rectangle tileRect = { (float)c * TILE_SIZE, (float)r * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
                    Rectangle overlap = GetCollisionRec(hitbox, tileRect);
                    float area = overlap.width * overlap.height;

                    if (area > maxArea) {
                        maxArea = area;
                        bestR = r;
                        bestC = c;
                    }
                }
            }
        }

        if (bestR != -1) {
            mapData[bestR][bestC] = 0;
            hitOccurred = true;
        }
    } else if (interactionType == 3) {
        // Collection logic: Can collect multiple items
        for (int r = startRow; r <= endRow; r++) {
            for (int c = startCol; c <= endCol; c++) {
                if (mapData[r][c] == 3) {
                    Rectangle tileRect = { (float)c * TILE_SIZE, (float)r * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
                    if (CheckCollisionRecs(hitbox, tileRect)) {
                        mapData[r][c] = 0;
                        hitOccurred = true;
                        std::cout << "Collectible picked up at [" << r << "," << c << "]" << std::endl;
                    }
                }
            }
        }
    }
    
    return hitOccurred;
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
            if (mapData[r][c] == 1 || mapData[r][c] == 2 || mapData[r][c] == 4 || mapData[r][c] == 5 || mapData[r][c] == 6 || mapData[r][c] == 7 || mapData[r][c] == 8 || mapData[r][c] == 9 || mapData[r][c] == 10 || mapData[r][c] == 11 || mapData[r][c] == 12 || mapData[r][c] == 13 || mapData[r][c] == 14 || mapData[r][c] == 15) {
                Rectangle tileRect = { (float)c * TILE_SIZE, (float)r * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
                if (CheckCollisionRecs(hitbox, tileRect)) {
                    return true;
                }
            }
        }
    }
    return false;
}
