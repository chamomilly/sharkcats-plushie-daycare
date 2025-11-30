#include <raylib.h>
#include <ctime>
#include <cstdlib>
#include "pet.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui-4.0/src/raygui.h"
#include "fish.h"

int main()
{
    InitWindow(350, 200, "Sharkcat's Plushie Daycare");
    SetTargetFPS(60);
    srand(time(nullptr));

    Pet pet = {
        {200, 175},
        {0, 0},
        IDLE,
        0.0f,
        Color{52, 103, 130, 255}};

    Fish fish{
        LoadTexture("resources/fish.png")};
    Vector2 fishPos = {-100, -100};
    float fishVelocity = 100;
    bool fishFalling = false;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        UpdatePet(pet, deltaTime);

        if (fishFalling)
        {
            // Check collision with pet
            float distance = sqrt(pow(fishPos.x - pet.position.x, 2) + pow(fishPos.y - pet.position.y, 2));
            if (distance < 25)
            {
                fishFalling = false;
                fishPos = {-100, -100};
                fishVelocity = 100;
            }

            // Apply gravity
            fishVelocity += 300 * deltaTime; // Gravity pulls down

            // Update position
            fishPos.y += fishVelocity * deltaTime;

            // Bounce off bottom
            if (fishPos.y > 180)
            {
                fishPos.y = 180;
                fishVelocity = -fishVelocity * 0.5f; // Bounce with energy loss

                // Stop if velocity is too small
                if (abs(fishVelocity) < 20)
                {
                    fishVelocity = 0;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawPet(pet);

        if (fishFalling)
        {
            DrawTextureEx(fish.texture, fishPos, 0.0f, 0.2f, WHITE);
        };

        if (GuiButton({10, 10, 80, 30}, "Feed"))
        {
            fishPos = {pet.position.x, 0};
            fishVelocity = 100;
            fishFalling = true;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
