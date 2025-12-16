#include <raylib.h>
#include <ctime>
#include <cstdlib>
#include "pet.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui-4.0/src/raygui.h"
#include "items/fish.h"
#include <raymath.h>
#include "items/ball.h"
#include <cstring>
#include "pet_choices.h"
#include "animation.h"
#include "animations.h"
#include "save_data.h"

int main()
{
    InitWindow(350, 200, "Plushie Daycare");
    SetExitKey(0); // Disable ESC key from closing window
    GuiLoadStyle("resources/candy.rgs");
    SetTargetFPS(60);
    srand(time(nullptr));
    Texture2D andrewSprite = LoadTexture("resources/andrew_sprite.png");
    Texture2D chilliSprite = LoadTexture("resources/chilli_sprite.png");

    RenderTexture2D andrewIcon = LoadRenderTexture(32, 32);
    BeginTextureMode(andrewIcon);
    ClearBackground(BLANK);
    DrawTexturePro(andrewSprite, {0, 0, 32, 32}, {0, 0, 32, 32}, {0, 0}, 0, WHITE);
    EndTextureMode();

    RenderTexture2D chilliIcon = LoadRenderTexture(32, 32);
    BeginTextureMode(chilliIcon);
    ClearBackground(BLANK);
    DrawTexturePro(chilliSprite, {0, 0, 32, 32}, {0, 0, 32, 32}, {0, 0}, 0, WHITE);
    EndTextureMode();

    PetChoice choices[2] = {
        {"Andrew",
         andrewIcon.texture},
        {"Chilli",
         chilliIcon.texture}};

    Pet pet = {
        {},                       // name (will be set below)
        {200, 125},               // position
        {0, 0},                   // velocity
        IDLE,                     // state
        0.0f,                     // state timer
        Color{52, 103, 130, 255}, // colour
        100.0f,                   // hunger (100 - full, 0 - starving)
        100.0f,                   // happiness
        100.0f,                   // energy
        0.0f,                     // jump offset
        false,                    // has pounced (so it only pounces once)
        {-100, -100},             // pounce target
        andrewSprite,             // sprite texture
        false                     // facingLeft
    };

    // Initialize animations with default values
    pet.idle = {0, 6, 0, 120, 0};
    pet.walk = {8 * 2, 5, 0, 120, 0};
    pet.sleep = {8 * 6, 8, 0, 120, 0};
    pet.pouncing = {8 * 3, 8, 0, 120, 0};

    Ball ball{
        {-100, -100},                      // position
        {0, 0},                            // velocity
        false,                             // active
        LoadTexture("resources/ball.png"), // texture
        8.0f,                              // radius
        0.0f,                              // rotation
        false                              // carried
    };

    Fish fish{

        {-100, -100},                     // position
        100,                              // velocity
        false,                            // isFalling
        LoadTexture("resources/fish.png") // texture
    };

    int selectedPetIndex = -1;
    bool showReferences = false;
    bool showQuitScreen = false;
    bool showResumeScreen = false;
    bool shouldQuit = false;

    SaveData saveData;
    if (LoadGame(saveData))
    {
        selectedPetIndex = saveData.selection;
        strcpy(pet.name, selectedPetIndex == 0 ? "Andrew" : "Chilli");
        pet.hunger = saveData.hunger;
        pet.happiness = saveData.happiness;
        pet.energy = saveData.energy;
        pet.sprite = selectedPetIndex == 0 ? andrewSprite : chilliSprite;

        if (strcmp(pet.name, "Andrew") == 0)
        {
            pet.idle = {0, 6, 0, 120, 0};
            pet.walk = {8 * 2, 5, 0, 120, 0};
            pet.pouncing = {8 * 3, 8, 0, 120, 0};
            pet.sleep = {8 * 6, 8, 0, 120, 0};
        }
        else
        {
            pet.idle = {0, 4, 0, 120, 0};
            pet.walk = {8 * 4, 8, 0, 120, 0};
            pet.pouncing = {8 * 8, 7, 0, 120, 0};
            pet.sleep = {8 * 6, 4, 0, 120, 0};
            pet.sprite = chilliSprite;
        }
        showResumeScreen = true;
    }

    while (showResumeScreen && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(Color{255, 250, 245, 255});

        DrawText("Welcome back!", 35, 20, 16, Color{252, 105, 85, 255});
        DrawText("Your plushie is still hungry!", 35, 38, 12, Color{252, 105, 85, 255});

        if (GuiButton({200, 60, 80, 20}, "Resume"))
        {
            showResumeScreen = false;
        }
        if (GuiButton({200, 90, 80, 20}, "New Game"))
        {
            selectedPetIndex = -1;
            strcpy(pet.name, "");
            pet.hunger = 100.0f;
            pet.happiness = 100.0f;
            pet.energy = 100.0f;
            showResumeScreen = false;
        }
        if (GuiButton({200, 120, 80, 20}, "Quit Game"))
        {
            shouldQuit = true;
            break;
        }

        if (selectedPetIndex >= 0)
        {
            Rectangle buttonRect = {50, 60, 60, 60};
            DrawRectangleRec(buttonRect, Color{254, 218, 150, 255});
            DrawRectangleLinesEx(buttonRect, 2, Color{229, 139, 104, 255});

            float iconSize = 56;
            float iconX = buttonRect.x + buttonRect.width / 2;
            float iconY = buttonRect.y + buttonRect.height / 2;

            DrawTexturePro(choices[selectedPetIndex].texture,
                           {0, 0, (float)choices[selectedPetIndex].texture.width, -(float)choices[selectedPetIndex].texture.height},
                           {iconX, iconY, iconSize, iconSize},
                           {iconSize / 2, iconSize / 2}, 0, WHITE);

            DrawText(choices[selectedPetIndex].name, buttonRect.x, buttonRect.y + buttonRect.height + 5, 10, Color{252, 105, 85, 255});
        }

        EndDrawing();
    }

    while (selectedPetIndex == -1 && !WindowShouldClose() && !shouldQuit)
    {
        while (!showReferences && selectedPetIndex == -1 && !WindowShouldClose())
        {
            BeginDrawing();
            int tileSize = 16;
            for (int x = 0; x < 350; x += tileSize)
            {
                for (int y = 0; y < 200; y += tileSize)
                {
                    Color color = ((x / tileSize + y / tileSize) % 2 == 0) ? Color{255, 250, 245, 255} : Color{245, 235, 225, 255};
                    DrawRectangle(x, y, tileSize, tileSize, color);
                }
            }

            DrawText("Welcome to Plushie Daycare!", 35, 20, 16, Color{252, 105, 85, 255});
            DrawText("Choose your plushie:", 35, 38, 12, Color{252, 105, 85, 255});

            Rectangle buttons[2];

            for (int i = 0; i < 2; i++)
            {
                Rectangle buttonRect = {35 + i * 70, 60, 60, 60};
                buttons[i] = buttonRect;

                DrawRectangleRec(buttonRect, Color{254, 218, 150, 255});
                DrawRectangleLinesEx(buttonRect, 2, Color{229, 139, 104, 255});

                float iconSize = 56;
                float iconX = buttonRect.x + buttonRect.width / 2;
                float iconY = buttonRect.y + buttonRect.height / 2;

                DrawTexturePro(choices[i].texture,
                               {0, 0, (float)choices[i].texture.width, -(float)choices[i].texture.height},
                               {iconX, iconY, iconSize, iconSize},
                               {iconSize / 2, iconSize / 2}, 0, WHITE);

                DrawText(choices[i].name, buttonRect.x, buttonRect.y + buttonRect.height + 5, 10, Color{252, 105, 85, 255});

                Vector2 mouse = GetMousePosition();
                if (CheckCollisionPointRec(mouse, buttonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    selectedPetIndex = i;
                    strcpy(pet.name, choices[i].name);
                    if (strcmp(choices[i].name, "Andrew") == 0)
                    {
                        pet.idle = {0, 6, 0, 120, 0};
                        pet.walk = {8 * 2, 5, 0, 120, 0};
                        pet.pouncing = {8 * 3, 8, 0, 120, 0};
                        pet.sleep = {8 * 6, 8, 0, 120, 0};
                    }
                    else
                    {
                        pet.idle = {0, 4, 0, 120, 0};
                        pet.walk = {8 * 4, 8, 0, 120, 0};
                        pet.pouncing = {8 * 8, 7, 0, 120, 0};
                        pet.sleep = {8 * 6, 4, 0, 120, 0};
                        pet.sprite = chilliSprite;
                    }
                }
            }

            Vector2 mouse = GetMousePosition();
            bool overInteractive = false;
            for (int i = 0; i < 2; i++)
            {
                if (CheckCollisionPointRec(mouse, buttons[i]))
                {
                    overInteractive = true;
                    break;
                }
            }

            DrawText("Chamomilly 2025", 265, 185, 8, Color{229, 139, 104, 255});
            if (GuiButton({20, 175, 100, 20}, "References"))
            {
                showReferences = true;
            }
            if (CheckCollisionPointRec(mouse, {20, 175, 100, 8}))
            {
                overInteractive = true;
            }

            SetMouseCursor(overInteractive ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);

            EndDrawing();
        }

        while (showReferences && !WindowShouldClose())
        {
            BeginDrawing();
            int tileSize = 16;
            for (int x = 0; x < 350; x += tileSize)
            {
                for (int y = 0; y < 200; y += tileSize)
                {
                    Color color = ((x / tileSize + y / tileSize) % 2 == 0) ? Color{255, 250, 245, 255} : Color{245, 235, 225, 255};
                    DrawRectangle(x, y, tileSize, tileSize, color);
                }
            }

            DrawText("Plushie Daycare References", 15, 20, 16, Color{252, 105, 85, 255});
            DrawText("Cat Sprites by Elthen's Pixel Art Shop", 15, 38, 12, Color{252, 105, 85, 255});
            DrawText("https://elthen.itch.io/2d-pixel-art-cat-sprites", 15, 52, 8, Color{252, 105, 85, 255});
            DrawText("Red Panda Sprites by Elthen's Pixel Art Shop", 15, 67, 12, Color{252, 105, 85, 255});
            DrawText("https://elthen.itch.io/2d-pixel-art-red-panda-sprites", 15, 80, 8, Color{252, 105, 85, 255});

            if (GuiButton({15, 100, 60, 20}, "Back"))
            {
                showReferences = false;
                selectedPetIndex = -1;
            }

            EndDrawing();
        }
    }

    while (!WindowShouldClose() && !shouldQuit)
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            showQuitScreen = true;
        }

        if (showQuitScreen)
        {
            BeginDrawing();
            int tileSize = 16;
            for (int x = 0; x < 350; x += tileSize)
            {
                for (int y = 0; y < 200; y += tileSize)
                {
                    Color color = ((x / tileSize + y / tileSize) % 2 == 0) ? Color{255, 250, 245, 255} : Color{245, 235, 225, 255};
                    DrawRectangle(x, y, tileSize, tileSize, color);
                }
            }

            DrawText("Are you sure you want to quit?", 50, 60, 16, Color{252, 105, 85, 255});
            DrawText("Your plushie will be saved.", 90, 90, 12, Color{252, 105, 85, 255});

            if (GuiButton({100, 120, 60, 20}, "Yes"))
            {
                SaveData saveData = {
                    strcmp(pet.name, "Andrew") == 0 ? 0 : 1,
                    (int)pet.happiness,
                    (int)pet.hunger,
                    (int)pet.energy};
                SaveGame(saveData);
                break;
            }
            if (GuiButton({180, 120, 60, 20}, "No"))
            {
                showQuitScreen = false;
            }

            EndDrawing();
            continue;
        }

        float deltaTime = GetFrameTime();

        UpdatePet(pet, deltaTime, ball);

        if (fish.isFalling)
        {
            // check collision with pet
            float distance = sqrt(pow(fish.position.x - pet.position.x, 2) + pow(fish.position.y - pet.position.y, 2));
            if (distance < 25)
            {
                fish.isFalling = false;
                fish.position = {-100, -100};
                fish.velocity = 100;

                pet.hunger += 30;
                pet.happiness += 10;

                pet.hunger = Clamp(pet.hunger, 0.0f, 100.0f);
                pet.happiness = Clamp(pet.happiness, 0.0f, 100.0f);
            }

            fish.velocity += 300 * deltaTime;

            fish.position.y += fish.velocity * deltaTime;

            if (fish.position.y > 180)
            {
                fish.position.y = 180;
                fish.velocity = -fish.velocity * 0.5f;

                if (abs(fish.velocity) < 20)
                {
                    fish.velocity = 0;
                }
            }
        }

        if (ball.active)
        {
            float dt = GetFrameTime();
            ball.velocity.y += 400 * dt;

            ball.position.x += ball.velocity.x * dt;
            ball.position.y += ball.velocity.y * dt;

            ball.rotation += ball.velocity.x * dt * 4.0f;
            if (ball.rotation > 360)
                ball.rotation -= 360;
            if (ball.rotation < 0)
                ball.rotation += 360;

            if (ball.position.y > 180)
            {
                ball.position.y = 180;
                ball.velocity.y = -ball.velocity.y * 0.5f;
            }

            ball.velocity.x *= 0.98f; // adding horizontal friction

            if (fabs(ball.velocity.x) < 5 && fabs(ball.velocity.y) < 5 && ball.position.y > 170)
            {
                ball.active = false;
            } // stop once slow enough
        }

        // Check cursor over interactive elements
        Vector2 mouse = GetMousePosition();
        bool overInteractive = false;

        // Check if over buttons
        if (CheckCollisionPointRec(mouse, {10, 30, 60, 20}) ||
            CheckCollisionPointRec(mouse, {80, 30, 60, 20}))
        {
            overInteractive = true;
        }

        // Check if over pet
        Vector2 petCenter = {pet.position.x, pet.position.y};
        if (Vector2Distance(mouse, petCenter) < 80)
        {
            overInteractive = true;
        }

        SetMouseCursor(overInteractive ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);

        // --- DRAWING BLOCK ---

        BeginDrawing();

        // Draw pixelated background
        int tileSize = 16;
        for (int x = 0; x < 350; x += tileSize)
        {
            for (int y = 0; y < 200; y += tileSize)
            {
                Color color = ((x / tileSize + y / tileSize) % 2 == 0) ? Color{255, 250, 245, 255} : Color{245, 235, 225, 255};
                DrawRectangle(x, y, tileSize, tileSize, color);
            }
        }

        DrawPet(pet);

        if (fish.isFalling)
        {
            DrawTextureEx(fish.texture, fish.position, 0.0f, 0.2f, WHITE);
        };

        DrawText(pet.name, 15, 10, 14, Color{252, 105, 85, 255});
        // maybe put icons here??

        if (GuiButton({10, 30, 60, 20}, "Feed"))
        {
            fish.position = {pet.position.x, 0};
            fish.velocity = 100;
            fish.isFalling = true;
        }
        if (GuiButton({80, 30, 60, 20}, "Play"))
        {
            Vector2 target = pet.position;
            ball.position = {50, 70};

            ball.active = true;

            Vector2 direction = Vector2Normalize(Vector2Subtract(target, ball.position));

            ball.velocity.x = direction.x * 280;
            ball.velocity.y = -220;

            if (pet.energy > 10)
            {
                pet.state = PLAYING;
                pet.happiness += 15;
                pet.energy -= 10;
            }
        }

        if (ball.active)
        {
            float scale = 0.05f;

            Rectangle src = {0, 0, (float)ball.texture.width, (float)ball.texture.height};
            Rectangle dst = {
                ball.position.x,
                ball.position.y,
                ball.texture.width * scale,
                ball.texture.height * scale};
            Vector2 origin = {dst.width / 2, dst.height / 2};

            DrawTexturePro(ball.texture, src, dst, origin, ball.rotation, WHITE);
        }

        float hungerValue = pet.hunger / 100.0f;
        float happinessValue = pet.happiness / 100.0f;
        float energyValue = pet.energy / 100.0f;

        GuiGroupBox({180, 5, 160, 65}, "Stats");
        GuiProgressBar({250, 10, 80, 15}, "Hunger", NULL, &hungerValue, 0.0f, 1.0f);
        GuiProgressBar({250, 30, 80, 15}, "Happiness", NULL, &happinessValue, 0.0f, 1.0f);
        GuiProgressBar({250, 50, 80, 15}, "Energy", NULL, &energyValue, 0.0f, 1.0f);

        EndDrawing();
    }

    UnloadRenderTexture(andrewIcon);
    CloseWindow();
    return 0;
}
