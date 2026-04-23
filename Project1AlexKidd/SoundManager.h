#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include "raylib.h"

class SoundManager {
public:
    SoundManager();
    ~SoundManager();

    void PlayJump();
    void PlayPunch();
    void PlayCoin();
    void PlayPlayerDeath();
    void PlayEnemyDeath();
    void PlayBlockBreak();
    void PlayStarBlockBreak();
    void PlayGameOver();

private:
    Sound jumpSound;
    Sound punchSound;
    Sound coinSound;
    Sound playerDeathSound;
    Sound enemyDeathSound;
    Sound blockBreakSound;
    Sound starBlockBreakSound;
    Sound gameOverSound;
};

#endif // SOUND_MANAGER_H
