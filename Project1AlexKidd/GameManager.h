#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "PlayerManager.h"
#include "MapManager.h"
#include "SoundManager.h"

class GameManager {
public:
    GameManager();
    ~GameManager();

    void Update();
    void Draw();

    PlayerManager* GetPlayerManager() { return player; }
    MapManager* GetMapManager() { return map; }
    SoundManager* GetSoundManager() { return sound; }

private:
    PlayerManager* player;
    MapManager* map;
    SoundManager* sound;
    bool showDebugHitboxes = false;
};

#endif // GAME_MANAGER_H
