#include "raylib.h"
#include <cmath>
#include <algorithm>
#include "GameManager.h"
#include "MapManager.h"

int main() {
    const int virtualWidth = 256;
    const int virtualHeight = 192;
    
    // Initial window size (3x scale)
    int windowWidth = 768;
    int windowHeight = 576;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "Alex Kidd Prototype");
    SetWindowMinSize(virtualWidth, virtualHeight);

    GameManager game;
    
    // Create a render texture for virtual resolution
    RenderTexture2D target = LoadRenderTexture(virtualWidth, virtualHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    Camera2D camera = { 0 };
    camera.target = game.GetPlayerManager()->GetPosition();
    camera.offset = { (float)virtualWidth / 2.0f, (float)virtualHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f; // Zoom is 1.0 because we're rendering to a small texture now

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Update window dimensions
        windowWidth = GetScreenWidth();
        windowHeight = GetScreenHeight();

        // Calculate scale factor (largest integer multiple)
        float scale = std::min((float)windowWidth / virtualWidth, (float)windowHeight / virtualHeight);
        scale = std::max(1.0f, floorf(scale)); // Integer scaling

        // Calculate render destination rectangle (centered)
        Rectangle dest = {
            (windowWidth - (virtualWidth * scale)) * 0.5f,
            (windowHeight - (virtualHeight * scale)) * 0.5f,
            virtualWidth * scale,
            virtualHeight * scale
        };

        // Update
        game.Update();

        // Update Camera
        Vector2 playerPos = game.GetPlayerManager()->GetPosition();
        camera.target.x = (float)round(playerPos.x);
        camera.target.y = (float)round(playerPos.y);

        // Camera Clamping (Virtual Resolution)
        float minX = camera.offset.x;
        float maxX = WORLD_WIDTH - camera.offset.x;
        float minY = camera.offset.y;
        float maxY = WORLD_HEIGHT - camera.offset.y;

        if (camera.target.x < minX) camera.target.x = minX;
        if (camera.target.x > maxX) camera.target.x = maxX;
        if (camera.target.y < minY) camera.target.y = minY;
        if (camera.target.y > maxY) camera.target.y = maxY;

        // Draw to virtual target
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);
                game.Draw();
            EndMode2D();
        EndTextureMode();

        // Draw virtual target to screen
        BeginDrawing();
            ClearBackground(BLACK); // Letterbox/Pillarbox color
            
            // Draw texture with flip (Raylib textures are Y-inverted in texture mode)
            DrawTexturePro(target.texture, 
                { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                dest, { 0, 0 }, 0.0f, WHITE);

            // Optional: Draw UI or overlay on top of the scaled texture
            // DrawText(TextFormat("Scale: %ix", (int)scale), 10, 10, 10, GREEN);
        EndDrawing();
    }

    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}
