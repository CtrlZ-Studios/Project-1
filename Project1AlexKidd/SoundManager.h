#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include "raylib.h"

class SoundManager {
public:
    SoundManager();
    ~SoundManager();

    void Update(); // To update music stream
    void PlayTheme();

private:
    Music themeMusic;
    bool musicLoaded;
};

#endif // SOUND_MANAGER_H
