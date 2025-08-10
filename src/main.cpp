#include "raylib.h"
#include "raymath.h"

#include <iostream>
#include <vector>


const double c = 299792458.0f; // Speed of light in m/s
const double G = 6.67430e-11f; // Gravitational constant in m^3 kg^-1 s^-2
const double VIS_SCALE = 6e-9; // Visualization scale: meters to pixels

const double TIME_MULTIPLIER = 100;

struct BlackHole
{
    Vector2 pos;
    double mass;

    double r_s; // Schwarzschild radius

    BlackHole(Vector2 position, double m)
     : pos(position), mass(m)
    {
        r_s = (2 * G * mass) / (c * c); // Calculate Schwarzschild radius
    }

    void Draw() const
    {
        // Scale Schwarzschild radius for visualization
        float scaled_r_s = static_cast<float>(r_s * VIS_SCALE);
        DrawCircleV(pos, scaled_r_s, RED); // Draw the black hole as a circle with scaled radius
    }
};

struct LightRay
{
    Vector2 pos;
    Vector2 dir;

    std::vector<Vector2> path;

    LightRay(Vector2 position = {0, 0}, Vector2 direction = {1, 0})
     : pos(position), dir(direction)
    {
        path.push_back(pos); // Initialize path with the starting position
    }

    void Draw() const
    {
        DrawCircleV(pos, 2.0f, WHITE); // Draw the current position

        const size_t N = path.size();
        for (size_t i = 0; i < path.size() - 1; ++i)
        {
            // Fade from white (newest) to black (oldest)
            float t = static_cast<float>(i) / (N - 1);
            Color fadeColor = {
                static_cast<unsigned char>(255 * (t - 1.0f)),
                static_cast<unsigned char>(255 * (t - 1.0f)),
                static_cast<unsigned char>(255 * (t - 1.0f)),
                255
            };
            DrawLineV(path[i], path[i + 1], fadeColor);
        }
    }

    void Update(double dt)
    {
        // Update position based on direction and speed of light, scaled for visualization
        pos.x += dir.x * static_cast<float>(c * dt * VIS_SCALE);
        pos.y += dir.y * static_cast<float>(c * dt * VIS_SCALE);

        path.push_back(pos);
    }
};

struct Simulation
{
    std::vector<BlackHole> blackHoles;
    std::vector<LightRay> lightRays;

    void Init(int width, int height)
    {
        BlackHole SagA(Vector2{width / 2.0f, height / 2.0f}, 8.54e36); // Sagittarius A* mass in kg
        blackHoles.push_back(SagA);

        int numRays = 20; // Number of light rays to simulate
        int step = height / numRays;
        for (int i = 0; i < height; i += step)
        {
            LightRay ray(Vector2{0, static_cast<float>(i + step / 2)}, Vector2{1, 0}); // Light ray starting from the left side
            lightRays.push_back(ray);
        }
    }

    void Update(double dt)
    {
        for (auto& lr : lightRays)
        {
            // Check for interactions with black holes
            bool absorbed = false;
            for (const auto& bh : blackHoles)
            {
                // Calculate distance to black hole
                double distance = Vector2Distance(lr.pos, bh.pos) / VIS_SCALE;

                // Check if the light ray is within the Schwarzschild radius
                if (distance < bh.r_s)
                {
                    // Light ray is absorbed by the black hole, stop updating it
                    absorbed = true;
                    break;
                }
            }

            if (!absorbed) lr.Update(dt); // Update each light ray's position
        }
    }

    void Draw()
    {
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw black holes
        for (const auto& bh : blackHoles)
        {
            bh.Draw();
        }

        // Draw light rays
        for (const auto& lr : lightRays)
        {
            lr.Draw();
        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    void Run()
    {
        while (!WindowShouldClose())
        {
            Update(GetFrameTime() * TIME_MULTIPLIER);

            Draw();
        }
    }
};

int main()
{
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Black Hole Visualization");
    SetTargetFPS(60);

    // Simulation Setup
    Simulation sim;
    sim.Init(screenWidth, screenHeight);
    sim.Run();

    CloseWindow();
    return 0;
}
