#include "MapManager.h"
#include "raylib.h"
#include <iostream>
#include "PlayerManager.h"

MapManager::MapManager() : tilesetLoaded(false), spawnPosition({100, 150}) {
    // Scan for spawn marker (-1) and enemies (-2, -3)
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            if (mapData[r][c] == -1) {
                // Align feet to the bottom of the tile
                spawnPosition = {
                    (float)c * TILE_SIZE + TILE_SIZE / 2.0f,
                    (float)r * TILE_SIZE + TILE_SIZE - PlayerManager::frameHeight
                };
                mapData[r][c] = 0; // Clear the marker
            } else if (mapData[r][c] == -2 || mapData[r][c] == -3) {
                enemySpawns.push_back({
                    mapData[r][c],
                    {(float)c * TILE_SIZE, (float)r * TILE_SIZE}
                });
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
                    Rectangle source = { 0, 0, 16, 16 }; 
                    
                    switch (mapData[r][c]) {
                        case 1: source = { 16, 0, 16, 16 }; break;  // Grass block
                        case 2: source = {0, 48, 16, 16}; break; // Destructible block
                        case 3: source = {16, 48, 16, 16}; break; // Big money block
                        case 4: source = {32, 16, 16, 16}; break; // Left wall
                        case 5: source = {0, 16, 16, 16}; break; // Right wall
                        case 6: source = {0, 0, 16, 16}; break; // Left corner grassy
                        case 7: source = {32, 0, 16, 16}; break; // Right corner grassy
                        case 8: source = {0, 32, 16, 16}; break; // Left corner
                        case 9: source = {32, 32, 16, 16}; break; // Right corner
                        case 10: source = {16, 32, 16, 16}; break; // Dirt
                        case 11: source = {16, 16, 16, 16}; break; // Barrier block
                        case 12: source = {32, 128, 16, 16}; break; // Grass_2_top
                        case 13: source = {0, 144, 16, 16}; break; // Grass_2_block
                        case 14: source = {0, 128, 16, 16}; break; // Red Ball
                        case 15: source = {16, 128, 16, 16}; break; // Blue Ball
                        case 16: source = {16, 160, 16, 16}; break; // Left side of cloud
                        case 17: source = {32, 160, 16, 16}; break; // Right side of cloud
                        case 18: source = {0, 176, 16, 16}; break; // Trunk of the tree
                        case 19: source = {16, 176, 16, 16}; break; // Leaves of the tree
                        case 20: source = {32, 176, 16, 16}; break; // Top of the tree
                        case 21: source = {0, 160, 16, 16}; break; // Small money block
                        case 22: source = {32, 48, 16, 16}; break; // Star block
                        case 23: source = {0, 64, 16, 16}; break; // Stun block
                    }
                    DrawTextureRec(tileset, source, pos, WHITE);
                }
            }
        }
    }
}

bool MapManager::InteractWithMap(Rectangle hitbox, int interactionType) {
    bool hitOccurred = false;
    
    int startCol = (int)floorf(hitbox.x / TILE_SIZE);
    int endCol = (int)floorf((hitbox.x + hitbox.width) / TILE_SIZE);
    int startRow = (int)floorf(hitbox.y / TILE_SIZE);
    int endRow = (int)floorf((hitbox.y + hitbox.height) / TILE_SIZE);

    if (startCol < 0) startCol = 0;
    if (endCol >= MAP_COLS) endCol = MAP_COLS - 1;
    if (startRow < 0) startRow = 0;
    if (endRow >= MAP_ROWS) endRow = MAP_ROWS - 1;

    if (interactionType == 2) {
        int bestR = -1;
        int bestC = -1;
        float maxArea = 0.0f;

        for (int r = startRow; r <= endRow; r++) {
            for (int c = startCol; c <= endCol; c++) {
                if (mapData[r][c] == 2 || mapData[r][c] == 15 || mapData[r][c] == 22 || mapData[r][c] == 23) {
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
        for (int r = startRow; r <= endRow; r++) {
            for (int c = startCol; c <= endCol; c++) {
                if (mapData[r][c] == 3 || mapData[r][c] == 21) {
                    Rectangle tileRect = { (float)c * TILE_SIZE, (float)r * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
                    if (CheckCollisionRecs(hitbox, tileRect)) {
                        mapData[r][c] = 0;
                        hitOccurred = true;
                    }
                }
            }
        }
    }
    
    return hitOccurred;
}

bool MapManager::CheckCollision(Rectangle hitbox) const {
    int startCol = (int)floorf(hitbox.x / TILE_SIZE);
    int endCol = (int)floorf((hitbox.x + hitbox.width) / TILE_SIZE);
    int startRow = (int)floorf(hitbox.y / TILE_SIZE);
    int endRow = (int)floorf((hitbox.y + hitbox.height) / TILE_SIZE);

    if (startCol < 0) startCol = 0;
    if (endCol >= MAP_COLS) endCol = MAP_COLS - 1;
    if (startRow < 0) startRow = 0;
    if (endRow >= MAP_ROWS) endRow = MAP_ROWS - 1;

    for (int r = startRow; r <= endRow; r++) {
        for (int c = startCol; c <= endCol; c++) {
            if (mapData[r][c] >= 1 && mapData[r][c] <= 15 || mapData[r][c] >= 21 && mapData[r][c] <= 23) {
                Rectangle tileRect = { (float)c * TILE_SIZE, (float)r * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
                if (CheckCollisionRecs(hitbox, tileRect)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void MapManager::CullOffscreen(float leftEdgeX) {
    int lastColToDelete = (int)(leftEdgeX / TILE_SIZE) - 1;
    if (lastColToDelete < 0) return;
    if (lastColToDelete >= MAP_COLS) lastColToDelete = MAP_COLS - 1;

    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c <= lastColToDelete; c++) {
            mapData[r][c] = 0;
        }
    }
}
