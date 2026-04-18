#include "SoundManager.h"

SoundManager::SoundManager() : musicLoaded(false) {
    InitAudioDevice();
    
    // Load music stream from Sound/theme.wav
    themeMusic = LoadMusicStream("Sound/theme.wav");
    
    if (themeMusic.stream.buffer != nullptr) {
        musicLoaded = true;
        PlayMusicStream(themeMusic);
        SetMusicVolume(themeMusic, 1.0f);
    }
}

SoundManager::~SoundManager() {
    if (musicLoaded) {
        UnloadMusicStream(themeMusic);
    }
    CloseAudioDevice();
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
