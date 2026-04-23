#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "PlayerManager.h"
#include "MapManager.h"
#include "SoundManager.h"
#include "Enemy.h"
#include <vector>
#include <deque>

struct DroppedItem {
    int tileID;
    Vector2 position;
    Vector2 velocity;
    float pickupCooldown;
    bool isGrounded;
};

class GameManager {
public:
    GameManager();
    ~GameManager();

    void Update();
    void Draw();
    void PlayerDied();
    void EnterShop();
    void ExitShop();

    PlayerManager* GetPlayerManager() { return player; }
    MapManager* GetMapManager() { return map; }
    SoundManager* GetSoundManager() { return sound; }
    Camera2D& GetCamera() { return camera; }

    static const bool DYNAMIC_MONEY_DROPS = false;
    bool shop1UpBought = false;

    // Shop UI Configuration
    const float shopUIWidthBlocks = 4.5f;
    const float shopUIHeightBlocks = 0.5f;
    const float shopUIOffsetXBlocks = 0.0f;
    const float shopUIOffsetYBlocks = 1.5f;

private:
    void ClearEnemies(bool returningFromShop = false);
    void SpawnEnemies(bool returningFromShop = false);
    void RestartLevel();
    void UpdateCamera();
    void CullOffscreen();

    PlayerManager* player;
    MapManager* map;
    SoundManager* sound;
    std::vector<Enemy*> enemies;
    std::vector<DroppedItem> droppedItems;
    Camera2D camera;
    float maxCameraX;
    bool showDebugHitboxes = false;

    // Economy System
    int playerMoney;

    // Progression System
    int lives = 3;
    int score = 0;
    int previousLevel = 1;
    bool isGameOver = false;
    bool gameOverSoundPlayed = false;
    bool gameWon = false;

    // Menu System (Level 0)
    float menuTimer = 0.0f;
    int menuColorVariant = 0;
    float menuFlickerTimer = 0.0f;
};

#endif // GAME_MANAGER_H
