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

    PlayerManager* GetPlayerManager() { return player; }
    MapManager* GetMapManager() { return map; }
    SoundManager* GetSoundManager() { return sound; }
    Camera2D& GetCamera() { return camera; }

    static const bool DYNAMIC_MONEY_DROPS = false;

private:
    void ClearEnemies();
    void SpawnEnemies();
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
};

#endif // GAME_MANAGER_H
