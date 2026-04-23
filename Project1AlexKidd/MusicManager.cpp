#include "MusicManager.h"

MusicManager::MusicManager() : currentMusic(nullptr) {
    titleScreen = LoadTrack("Sound/01 - Title Screen.ogg");
    levelStart = LoadTrack("Sound/02 - Level Start.ogg");
    mainTheme = LoadTrack("Sound/03 - Main Theme.ogg");
    underwater = LoadTrack("Sound/04 - Underwater.ogg");
    sukopako = LoadTrack("Sound/05 - Sukopako Motorcycle.ogg");
    janken = LoadTrack("Sound/06 - Janken.ogg");
    peticopter = LoadTrack("Sound/07 - Peticopter.ogg");
    castle = LoadTrack("Sound/08 - Castle.ogg");
    ending = LoadTrack("Sound/09 - Ending.ogg");
    gameOver = LoadTrack("Sound/10 - Game Over.ogg");
    theme = LoadTrack("Sound/theme.wav");

    // Set non-looping tracks
    titleScreen.looping = false;
    levelStart.looping = false;
    ending.looping = false;
    
    // Looping tracks (default is true in many implementations, but explicitly set)
    mainTheme.looping = true;
    underwater.looping = true;
    sukopako.looping = true;
    janken.looping = true;
    peticopter.looping = true;
    castle.looping = true;
    gameOver.looping = true; // Not specified, but usually loops or stops. Assuming true for now unless specified.
    theme.looping = true;
}

MusicManager::~MusicManager() {
    UnloadMusicStream(titleScreen);
    UnloadMusicStream(levelStart);
    UnloadMusicStream(mainTheme);
    UnloadMusicStream(underwater);
    UnloadMusicStream(sukopako);
    UnloadMusicStream(janken);
    UnloadMusicStream(peticopter);
    UnloadMusicStream(castle);
    UnloadMusicStream(ending);
    UnloadMusicStream(gameOver);
    UnloadMusicStream(theme);
}

Music MusicManager::LoadTrack(const char* path) {
    return LoadMusicStream(path);
}

void MusicManager::Update() {
    if (currentMusic != nullptr) {
        UpdateMusicStream(*currentMusic);
    }
}

void MusicManager::Play(MusicTrack track) {
    Stop();
    switch (track) {
        case MusicTrack::TITLE_SCREEN: currentMusic = &titleScreen; break;
        case MusicTrack::LEVEL_START: currentMusic = &levelStart; break;
        case MusicTrack::MAIN_THEME: currentMusic = &mainTheme; break;
        case MusicTrack::UNDERWATER: currentMusic = &underwater; break;
        case MusicTrack::SUKOPAKO_MOTORCYCLE: currentMusic = &sukopako; break;
        case MusicTrack::JANKEN: currentMusic = &janken; break;
        case MusicTrack::PETICOPTER: currentMusic = &peticopter; break;
        case MusicTrack::CASTLE: currentMusic = &castle; break;
        case MusicTrack::ENDING: currentMusic = &ending; break;
        case MusicTrack::GAME_OVER: currentMusic = &gameOver; break;
        case MusicTrack::THEME: currentMusic = &theme; break;
    }
    if (currentMusic != nullptr) {
        PlayMusicStream(*currentMusic);
    }
}

void MusicManager::Stop() {
    if (currentMusic != nullptr) {
        StopMusicStream(*currentMusic);
        currentMusic = nullptr;
    }
}

bool MusicManager::IsCurrentTrackFinished() const {
    if (currentMusic == nullptr) return true;
    return !IsMusicStreamPlaying(*currentMusic);
}
