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
    jumpSound = LoadSound("Sound/01.wav");         // 01: Jump
    punchSound = LoadSound("Sound/03.wav");        // 03: Punch
    coinSound = LoadSound("Sound/08.wav");         // 08: Collect Baum (Money)
    playerDeathSound = LoadSound("Sound/35.wav");  // 35: Death
    enemyDeathSound = LoadSound("Sound/07.wav");   // 07: Small Enemy Defeated
    blockBreakSound = LoadSound("Sound/04.wav");   //04: Block Break
    starBlockBreakSound = LoadSound("Sound/05.wav"); //05: Star Block Break
    gameOverSound = LoadSound("Sound/10-GameOver.wav");     // 10: Game Over
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
    UnloadSound(blockBreakSound);
    UnloadSound(starBlockBreakSound);
    UnloadSound(gameOverSound);

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
void SoundManager::PlayBlockBreak() { PlaySound(blockBreakSound); }
void SoundManager::PlayStarBlockBreak() { PlaySound(starBlockBreakSound); }
void SoundManager::StopTheme() { if (musicLoaded) StopMusicStream(themeMusic); }
void SoundManager::PlayGameOver() { PlaySound(gameOverSound); }
