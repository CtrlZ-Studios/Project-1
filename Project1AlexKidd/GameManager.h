#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "PlayerManager.h"
#include "MapManager.h"
#include "SoundManager.h"
#include "Enemy.h"
#include <vector>
#include <deque>

struct DroppedItem {
    int tileID;
    Vector2 position;
    Vector2 velocity;
    float pickupCooldown;
    bool isGrounded;
};

class GameManager {
public:
    GameManager();
    ~GameManager();

    void Update();
    void Draw();
    void PlayerDied();
    void EnterShop();
    void ExitShop();

    PlayerManager* GetPlayerManager() { return player; }
    MapManager* GetMapManager() { return map; }
    SoundManager* GetSoundManager() { return sound; }
    Camera2D& GetCamera() { return camera; }

    static const bool DYNAMIC_MONEY_DROPS = false;
    bool shop1UpBought = false;

    // Shop UI Configuration
    const float shopUIWidthBlocks = 4.5f;
    const float shopUIHeightBlocks = 0.5f;
    const float shopUIOffsetXBlocks = 0.0f;
    const float shopUIOffsetYBlocks = 1.5f;

private:
    void ClearEnemies(bool returningFromShop = false);
    void SpawnEnemies(bool returningFromShop = false);
    void RestartLevel();
    void UpdateCamera();
    void CullOffscreen();

    PlayerManager* player;
    MapManager* map;
    SoundManager* sound;
    std::vector<Enemy*> enemies;
    std::vector<DroppedItem> droppedItems;
    Camera2D camera;
    float maxCameraX;
    bool showDebugHitboxes = false;

    // Economy System
    int playerMoney;

    // Progression System
    int lives = 3;
    int score = 0;
    int previousLevel = 1;
    bool isGameOver = false;
    bool gameOverSoundPlayed = false;
    bool gameWon = false;

    // Menu System (Level 0)
    float menuTimer = 0.0f;
    int menuColorVariant = 0;
    float menuFlickerTimer = 0.0f;

    // --- Pause Menu ---
    bool isPaused = false;

    // Pause textures
    Texture2D pauseMapTex;      // Sprites/pausemap.png
    Texture2D pauseLvl1Tex;     // Sprites/pauselvl1.png
    Texture2D pauseLvl2Tex;     // Sprites/pauselvl2.png
    Texture2D pauseUiTex;       // Sprites/pauseui.png
    bool pauseTexturesLoaded = false;

    // Flicker state
    float pauseFlickerTimer = 0.0f;
    const float pauseFlickerInterval = 0.5f; // seconds between flicker swap
    bool pauseFlickerState = false;           // false = pausemap, true = pauselvl

    // Text offsets (tweak these values freely later)
    const float pauseLivesOffsetX = 70.0f;
    const float pauseLivesOffsetY = 171.0f;
    const float pauseScoreOffsetX = 170.0f;
    const float pauseScoreOffsetY = 175.0f;
    const float pauseMoneyOffsetX = 70.0f;
    const float pauseMoneyOffsetY = 155.0f;

    // --- Shop Dialogue ---
    enum class ShopDialogue { WELCOME, SHORT_OF_MONEY, THANK_YOU };
    ShopDialogue currentShopDialogue = ShopDialogue::WELCOME;

    // Offset constants (tweak freely)
    const float shopDialogueY = 32.0f; // vertical center of box
    const float shopDialoguePadding = 6.0f;
    const int shopDialogueFontSize = 8;

    // --- Level 1 Story Dialogue ---
    bool storyDialogueActive = false;
    bool storyDialogueTriggered = false; // so it only fires once
    bool storyDialoguePreviousFrameHadInput = false;

    const char* storyDialogueText =
        "WELCOME, ALEX. YOU ARE A \n"
        "PRINCE FROM THE COUNTRY OF\n"
        "RADAXIAN, WHO WAS KIDNAPPED\n" 
        "BY EVIL MEN WHEN YOU WERE\n"
        "BUT A SMALL BOY. YOUR NATIVE\n" 
        "LAND IS NOW BEING GROSSLY\n"
        "MISGOVERNED BY THE TYRANT,\n"
        "\"JANKEN THE GREAT.\" YOUR\n"
        "MISSION IS TO SAVE THE\n"
        "POPULACE FROM HIM.";

    const int storyDialogueMargin = 16;
    const int storyDialogueFontSize = 8;
    const int storyDialogueLineSpacing = 12;

    void DrawPauseMenu(float deltaTime);
    void DrawShopDialogue();
    void DrawStoryDialogue();
};

#endif // GAME_MANAGER_H
