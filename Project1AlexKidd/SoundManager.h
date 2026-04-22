#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include "raylib.h"

class SoundManager {
public:
    SoundManager();
    ~SoundManager();

    void Update(); // To update music stream
    void PlayTheme();

    void PlayJump();
    void PlayPunch();
    void PlayCoin();
    void PlayPlayerDeath();
    void PlayEnemyDeath();

private:
    Music themeMusic;
    bool musicLoaded;

    Sound jumpSound;
    Sound punchSound;
    Sound coinSound;
    Sound playerDeathSound;
    Sound enemyDeathSound;
};

#endif // SOUND_MANAGER_H
