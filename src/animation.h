struct Animation
{
    int startFrame; // first frame index in the sheet
    int frameCount; // how many frames in this animation
    int currentFrame = 0;
    float frameTime; // ms per frame
    float timer = 0;
};

inline void UpdateAnimation(Animation &anim)
{
    anim.timer += GetFrameTime() * 1000;

    if (anim.timer >= anim.frameTime)
    {
        anim.timer = 0;
        anim.currentFrame++;

        if (anim.currentFrame >= anim.frameCount)
            anim.currentFrame = 0;
    }
}

inline void DrawAnimation(Texture2D tex, const Animation &anim, Vector2 pos)
{
    int frameIndex = anim.startFrame + anim.currentFrame;

    int frameX = (frameIndex % 8) * 32;
    int frameY = (frameIndex / 8) * 32;

    Rectangle src = {(float)frameX, (float)frameY, 32, 32};

    DrawTextureRec(tex, src, pos, WHITE);
}
