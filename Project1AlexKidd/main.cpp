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

        // Draw to virtual target
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);
            BeginMode2D(game.GetCamera());
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
        EndDrawing();
    }

    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}
