#include "raylib.h"

struct BlackHole
{
    Vector2 pos;
};

struct Simulation
{

    void Init(int width, int height)
    {
    }

    void Update(float dt)
    {
    }

    void Draw()
    {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawFPS(10, 10);
        EndDrawing();
    }

    void Run()
    {
        while (!WindowShouldClose())
        {
            Update(GetFrameTime());

            Draw();
        }   
    }
};

int main()
{
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "raylib + CMake + VS Code");
    SetTargetFPS(60);

    // Simulation Setup
    Simulation sim;
    sim.Init(screenWidth, screenHeight);
    sim.Run();

    CloseWindow();
    return 0;
}
