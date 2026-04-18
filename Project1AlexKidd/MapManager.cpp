#include "MapManager.h"

MapManager::MapManager() : tilesetLoaded(false) {
    tileset = LoadTexture("Sprites/tileset.png");
    if (tileset.id != 0) {
        tilesetLoaded = true;
    }

    // Define the floor at the bottom of the screen
    // The floor is 1 tile high (16 pixels) and spans the entire world width
    floorRect = { 0, (float)WORLD_HEIGHT - TILE_SIZE, (float)WORLD_WIDTH, (float)TILE_SIZE };
}

MapManager::~MapManager() {
    if (tilesetLoaded) {
        UnloadTexture(tileset);
    }
}

void MapManager::Draw() {
    // Draw tiles across the entire width
    int numTiles = WORLD_WIDTH / TILE_SIZE;
    
    for (int i = 0; i < numTiles; i++) {
        // Draw the first tile from tileset (assuming index 0 is a floor tile)
        if (tilesetLoaded) {
            DrawTextureRec(tileset, { 16, 0, (float)TILE_SIZE, (float)TILE_SIZE }, 
                           { (float)i * TILE_SIZE, (float)WORLD_HEIGHT - TILE_SIZE }, WHITE);
        } else {
            // Fallback if texture not found
            DrawRectangle(i * TILE_SIZE, WORLD_HEIGHT - TILE_SIZE, TILE_SIZE, TILE_SIZE, BROWN);
        }
    }
}

Rectangle MapManager::GetFloorBounds() const {
    return floorRect;
}
