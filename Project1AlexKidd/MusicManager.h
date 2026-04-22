#ifndef MUSIC_MANAGER_H
#define MUSIC_MANAGER_H

#include "raylib.h"

enum class MusicTrack {
    NONE,
    TITLE_SCREEN,
    LEVEL_START,
    MAIN_THEME,
    UNDERWATER,
    MOTORCYCLE,
    JANKEN,
    PETICOPTER,
    CASTLE,
    ENDING,
    GAME_OVER
};

class MusicManager {
public:
    MusicManager();
    ~MusicManager();

    void Update();
    void Play(MusicTrack track);
    void Stop();

    MusicTrack GetCurrentTrack() const { return currentTrack; }

private:
    Music LoadTrack(const char* filename);
    void StopCurrent();

    Music titleScreen;
    Music levelStart;
    Music mainTheme;
    Music underwater;
    Music motorcycle;
    Music janken;
    Music peticopter;
    Music castle;
    Music ending;
    Music gameOver;

    MusicTrack currentTrack;
    Music* currentMusic;
};

#endif
