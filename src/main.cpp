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

int main()
{
    InitWindow(350, 200, "Plushie Daycare");
    GuiLoadStyle("resources/candy.rgs");
    SetTargetFPS(60);
    srand(time(nullptr));
    Texture2D sprite = LoadTexture("resources/andrew_sprite.png");

    PetChoice choices[4] = {
        {"Shark Cat",
         LoadTexture("resources/sharkcat/sharkcat.png")},
        {"Andrew",
         LoadTexture("resources/andrew.png")},
        {"Toby", LoadTexture("resources/toby.png")},
        {"Bruce", LoadTexture("resources/bruce.png")}};

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
        sprite                    // sprite texture
    };

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

    Animation walk{
        8 * 2,
        5,
        0,
        120, 0};

    Animation idle{
        0,
        6,
        0,
        120,
        0};

    Animation idle2{
        8,
        6,
        0,
        120,
        0};

    Animation jump{
        8 * 3,
        8,
        0,
        120,
        0};

    Animation sleep{
        8 * 6,
        8,
        0,
        120,
        0};

    int selectedPetIndex = -1;
    while (selectedPetIndex == -1 && !WindowShouldClose())
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

        Rectangle buttons[4]; // to know if hovered over

        for (int i = 0; i < 4; i++)
        {
            Rectangle buttonRect = {35 + i * 70, 80, 60, 60};
            buttons[i] = buttonRect;

            DrawRectangleRec(buttonRect, Color{254, 218, 150, 255});
            DrawRectangleLinesEx(buttonRect, 2, Color{229, 139, 104, 255});

            DrawTexturePro(choices[i].texture,
                           {0, 0, (float)choices[i].texture.width, (float)choices[i].texture.height},
                           {buttonRect.x, buttonRect.y, buttonRect.width, buttonRect.height},
                           {0, 0}, 0, WHITE);

            DrawText(choices[i].name, buttonRect.x, buttonRect.y + buttonRect.height + 5, 10, Color{252, 105, 85, 255});

            Vector2 mouse = GetMousePosition();
            if (CheckCollisionPointRec(mouse, buttonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                selectedPetIndex = i;
                strcpy(pet.name, choices[i].name);
            }
        }

        DrawText("Chamomilly 2025", 265, 185, 8, Color{229, 139, 104, 255});

        Vector2 mouse = GetMousePosition();
        bool overInteractive = false;
        for (int i = 0; i < 4; i++)
        {
            if (CheckCollisionPointRec(mouse, buttons[i]))
            {
                overInteractive = true;
                break;
            }
        }
        SetMouseCursor(overInteractive ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);

        EndDrawing();
    }

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        UpdatePet(pet, deltaTime, ball, idle, walk, sleep, jump);

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

        DrawPet(pet, idle, walk, sleep, jump);

        if (fish.isFalling)
        {
            DrawTextureEx(fish.texture, fish.position, 0.0f, 0.2f, WHITE);
        };

        DrawRectangle(10, 5, 100, 21, Color{255, 255, 255, 200});
        DrawRectangleLines(10, 5, 100, 21, Color{180, 180, 180, 255});
        DrawText(pet.name, 15, 10, 12, Color{229, 139, 104, 255});
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

    CloseWindow();
    return 0;
}
