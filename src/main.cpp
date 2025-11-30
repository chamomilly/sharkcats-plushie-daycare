#include <raylib.h>
#include <ctime>
#include <cstdlib>
#include "pet.h"

int main()
{
    InitWindow(400, 300, "Sharkcat's Plushie Daycare");
    SetTargetFPS(60);
    srand(time(nullptr));
    
    Pet pet = {
        {200, 150}, // position
        {0, 0},     // velocity
        IDLE,       // state
        0.0f,       // timer
        PINK        // color
    };
    
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        
        UpdatePet(pet, deltaTime);
        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawPet(pet);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
