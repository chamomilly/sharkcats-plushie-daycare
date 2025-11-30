#include <raymath.h>
#include <raylib.h>

struct Ball
{
    Vector2 position;
    Vector2 velocity;
    bool active;
    Texture2D texture;
    float radius;
    float rotation;
    bool carried;
};