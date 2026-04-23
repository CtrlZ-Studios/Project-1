#ifndef MUSIC_MANAGER_H
#define MUSIC_MANAGER_H

#include "raylib.h"

enum class MusicTrack {
    TITLE_SCREEN,
    LEVEL_START,
    MAIN_THEME,
    UNDERWATER,
    SUKOPAKO_MOTORCYCLE,
    JANKEN,
    PETICOPTER,
    CASTLE,
    ENDING,
    GAME_OVER,
    THEME
};

class MusicManager {
public:
    MusicManager();
    ~MusicManager();

    void Update();
    void Play(MusicTrack track);
    void Stop();
    bool IsCurrentTrackFinished() const;

private:
    Music* currentMusic;

    Music titleScreen;
    Music levelStart;
    Music mainTheme;
    Music underwater;
    Music sukopako;
    Music janken;
    Music peticopter;
    Music castle;
    Music ending;
    Music gameOver;
    Music theme;

    Music LoadTrack(const char* path);
};

#endif
