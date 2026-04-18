#include "GameManager.h"

GameManager::GameManager() {
    player = new PlayerManager();
    map = new MapManager();
    sound = new SoundManager();
}

GameManager::~GameManager() {
    delete player;
    delete map;
    delete sound;
}

void GameManager::Update() {
    float dt = GetFrameTime();
    
    // Update Sound (Music Stream)
    sound->Update();
    
    // Update Player with Map collision
    player->Update(dt, map->GetFloorBounds());
}

void GameManager::Draw() {
    map->Draw();
    player->Draw();
}
