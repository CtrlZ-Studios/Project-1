#include "MusicManager.h"

Music MusicManager::LoadTrack(const char* filename) {
    Music music = LoadMusicStream(filename);
    SetMusicVolume(music, 1.0f);
    return music;
}

MusicManager::MusicManager() : currentTrack(MusicTrack::NONE), currentMusic(nullptr) {
    titleScreen = LoadTrack("Sound/01 - Title Screen.ogg");
    levelStart   = LoadTrack("Sound/02 - Level Start.ogg");
    mainTheme    = LoadTrack("Sound/03 - Main Theme.ogg");
    underwater   = LoadTrack("Sound/04 - Underwater.ogg");
    motorcycle   = LoadTrack("Sound/05 - Sukopako Motorcycle.ogg");
    janken       = LoadTrack("Sound/06 - Janken.ogg");
    peticopter   = LoadTrack("Sound/07 - Peticopter.ogg");
    castle       = LoadTrack("Sound/08 - Castle.ogg");
    ending       = LoadTrack("Sound/09 - Ending.ogg");
    gameOver     = LoadTrack("Sound/10 - Game Over.ogg");
}

MusicManager::~MusicManager() {
    StopCurrent();

    UnloadMusicStream(titleScreen);
    UnloadMusicStream(levelStart);
    UnloadMusicStream(mainTheme);
    UnloadMusicStream(underwater);
    UnloadMusicStream(motorcycle);
    UnloadMusicStream(janken);
    UnloadMusicStream(peticopter);
    UnloadMusicStream(castle);
    UnloadMusicStream(ending);
    UnloadMusicStream(gameOver);
}

void MusicManager::Update() {
    if (currentMusic != nullptr && IsMusicStreamPlaying(*currentMusic)) {
        UpdateMusicStream(*currentMusic);
    }
}

void MusicManager::Play(MusicTrack track) {
    if (track == currentTrack) return;

    StopCurrent();

    switch (track) {
        case MusicTrack::TITLE_SCREEN: currentMusic = &titleScreen;  break;
        case MusicTrack::LEVEL_START:  currentMusic = &levelStart;   break;
        case MusicTrack::MAIN_THEME:   currentMusic = &mainTheme;    break;
        case MusicTrack::UNDERWATER:   currentMusic = &underwater;   break;
        case MusicTrack::MOTORCYCLE:   currentMusic = &motorcycle;   break;
        case MusicTrack::JANKEN:       currentMusic = &janken;       break;
        case MusicTrack::PETICOPTER:   currentMusic = &peticopter;   break;
        case MusicTrack::CASTLE:       currentMusic = &castle;       break;
        case MusicTrack::ENDING:       currentMusic = &ending;       break;
        case MusicTrack::GAME_OVER:    currentMusic = &gameOver;     break;
        default:                       currentMusic = nullptr;       break;
    }

    if (currentMusic != nullptr) {
        PlayMusicStream(*currentMusic);
        currentTrack = track;
    }
}

void MusicManager::Stop() {
    StopCurrent();
    currentTrack = MusicTrack::NONE;
    currentMusic = nullptr;
}

void MusicManager::StopCurrent() {
    if (currentMusic != nullptr && IsMusicStreamPlaying(*currentMusic)) {
        StopMusicStream(*currentMusic);
    }
}
