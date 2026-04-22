#include "SoundManager.h"

SoundManager::SoundManager() : musicLoaded(false) {
    // DO NOT call InitAudioDevice() here anymore!
    
    // Load music stream from Sound/theme.wav
    themeMusic = LoadMusicStream("Sound/theme.wav");
    
    if (themeMusic.stream.buffer != nullptr) {
        musicLoaded = true;
        PlayMusicStream(themeMusic);
        SetMusicVolume(themeMusic, 1.0f);
    }

    // Load SFX
    jumpSound = LoadSound("Sound/jump.wav");
    punchSound = LoadSound("Sound/punch.wav");
    coinSound = LoadSound("Sound/coin.wav");
    playerDeathSound = LoadSound("Sound/death.wav");
    enemyDeathSound = LoadSound("Sound/enemy_death.wav");
}

SoundManager::~SoundManager() {
    if (musicLoaded) {
        UnloadMusicStream(themeMusic);
    }

    UnloadSound(jumpSound);
    UnloadSound(punchSound);
    UnloadSound(coinSound);
    UnloadSound(playerDeathSound);
    UnloadSound(enemyDeathSound);

    // DO NOT call CloseAudioDevice() here anymore!
}

void SoundManager::Update() {
    if (musicLoaded) {
        UpdateMusicStream(themeMusic);
    }
}

void SoundManager::PlayTheme() {
    if (musicLoaded && !IsMusicStreamPlaying(themeMusic)) {
        PlayMusicStream(themeMusic);
    }
}

void SoundManager::PlayJump() { PlaySound(jumpSound); }
void SoundManager::PlayPunch() { PlaySound(punchSound); }
void SoundManager::PlayCoin() { PlaySound(coinSound); }
void SoundManager::PlayPlayerDeath() { PlaySound(playerDeathSound); }
void SoundManager::PlayEnemyDeath() { PlaySound(enemyDeathSound); }
