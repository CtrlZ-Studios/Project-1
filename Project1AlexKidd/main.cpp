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
    camera.offset = { (float)screenWidth / 2, (float)screenHeight / 2 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Update
        game.Update();

        // Update Camera
        Vector2 playerPos = game.GetPlayerManager()->GetPosition();
        camera.target = playerPos;

        // Camera Clamping
        // Prevent camera from showing area outside [0, WORLD_WIDTH] and [0, WORLD_HEIGHT]
        
        // Horizontal clamping
        if (camera.target.x < screenWidth / 2.0f) {
            camera.target.x = screenWidth / 2.0f;
        } else if (camera.target.x > WORLD_WIDTH - screenWidth / 2.0f) {
            camera.target.x = WORLD_WIDTH - screenWidth / 2.0f;
        }

        // Vertical clamping
        if (camera.target.y < screenHeight / 2.0f) {
            camera.target.y = screenHeight / 2.0f;
        } else if (camera.target.y > WORLD_HEIGHT - screenHeight / 2.0f) {
            camera.target.y = WORLD_HEIGHT - screenHeight / 2.0f;
        }

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
