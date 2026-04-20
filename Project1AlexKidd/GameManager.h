#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "PlayerManager.h"
#include "MapManager.h"
#include "SoundManager.h"
#include "Enemy.h"
#include <vector>

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
    Camera2D camera;
    float maxCameraX;
    bool showDebugHitboxes = false;
};

#endif // GAME_MANAGER_H
