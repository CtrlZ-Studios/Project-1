#include "raylib.h"
#include "GameManager.h"
#include "MapManager.h" // For WORLD_WIDTH, WORLD_HEIGHT

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Alex Kidd Prototype");

    GameManager game;
    
    Camera2D camera = { 0 };
    camera.target = game.GetPlayerManager()->GetPosition();
    // Offset centers the player on screen, accounting for the zoom factor
    camera.offset = { (float)screenWidth / 2.0f - (PlayerManager::frameWidth * 3.0f / 2.0f), 
                      (float)screenHeight / 2.0f - (PlayerManager::frameHeight * 3.0f / 2.0f) };
    camera.rotation = 0.0f;
    camera.zoom = 3.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Update
        game.Update();

        // Update Camera
        Vector2 playerPos = game.GetPlayerManager()->GetPosition();
        camera.target = playerPos;

        // Camera Clamping
        // Prevent camera from showing area outside [0, WORLD_WIDTH] and [0, WORLD_HEIGHT]
        // The visible area in world space depends on the zoom factor
        float minX = camera.offset.x / camera.zoom;
        float maxX = WORLD_WIDTH - (screenWidth - camera.offset.x) / camera.zoom;
        float minY = camera.offset.y / camera.zoom;
        float maxY = WORLD_HEIGHT - (screenHeight - camera.offset.y) / camera.zoom;

        if (camera.target.x < minX) camera.target.x = minX;
        if (camera.target.x > maxX) camera.target.x = maxX;
        if (camera.target.y < minY) camera.target.y = minY;
        if (camera.target.y > maxY) camera.target.y = maxY;

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode2D(camera);
                game.Draw();
            EndMode2D();

            // UI or debug info could go here (outside camera)
            DrawText("A/D to Move, SPACE to Jump", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
