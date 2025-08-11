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
};

struct LightRay
{
    Vector2 pos; // cartesian
    Vector2 dir;

    double r, phi; // polar
    double dr = 0.0, dphi = 0.0;

    std::vector<Vector2> path;

    LightRay(Vector2 position = {0, 0}, Vector2 direction = {1, 0})
        : pos(position), dir(direction)
    {
        r = hypot(pos.x, pos.y) / VIS_SCALE;
        phi = atan2(pos.y, pos.x); // Calculate initial angle

        path.push_back(pos); // Initialize path with the starting position
    }

    void Update(double dt, double r_s)
    {
        r = hypot(pos.x, pos.y) / VIS_SCALE;
        phi = atan2(pos.y, pos.x);

        // Safety check
        if (r <= 0)
            return;

        if (r < r_s)
        {
            // Light ray is within the Schwarzschild radius, it is absorbed
            return;
        }

        // Convert Cartesian velocity to polar coordinates
        // pos = (r*cos(phi), r*sin(phi)) in scaled coordinates
        Vector2 r_hat = {static_cast<float>(cos(phi)), static_cast<float>(sin(phi))};
        Vector2 phi_hat = {static_cast<float>(-sin(phi)), static_cast<float>(cos(phi))};

        // Current velocities in polar coordinates
        double v_r = (dir.x * r_hat.x + dir.y * r_hat.y) * c;       // Radial velocity
        double v_phi = (dir.x * phi_hat.x + dir.y * phi_hat.y) * c; // Tangential velocity

        // Convert tangential velocity to angular velocity: v_phi = r * dphi/dt
        double dphi_dt = v_phi / r;
        double dr_dt = v_r;

        // Geodesic equations for light rays in Schwarzschild metric
        // These are the exact equations of motion for photons

        // For light rays, we use the affine parameter λ instead of coordinate time t
        // But for numerical integration, we can use coordinate time with proper scaling

        // The key conserved quantities for light rays:
        // 1. Energy: E (related to dt/dλ)
        // 2. Angular momentum: L = r² * dφ/dλ

        // Since we're already in motion, we can calculate L from current state
        double L = r * r * dphi_dt / c; // Angular momentum per unit energy

        // Geodesic equations in Schwarzschild coordinates for light (ds² = 0):
        // d²r/dλ² = -GM/r² + L²/r³ - 3GM*L²/r⁴
        // d²φ/dλ² = -2/r * dr/dλ * dφ/dλ

        // Converting to coordinate time derivatives using r_s = 2GM/c²:
        // d²r/dt² = -(r_s*c²)/(2*r²) + L²*c²/r³ - (3*r_s*L²*c²)/(2*r⁴)
        // d²φ/dt² = -2/r * dr/dt * dφ/dt

        double r2 = r * r;
        double r3 = r2 * r;
        double r4 = r3 * r;
        double c2 = c * c;

        // Second derivatives (accelerations)
        double d2r_dt2 = -(r_s * c2) / (2.0 * r2) + (L * L * c2) / r3 - (3.0 * r_s * L * L * c2) / (2.0 * r4);
        double d2phi_dt2 = (-2.0 / r) * dr_dt * dphi_dt;

        // Update velocities first
        dr_dt += d2r_dt2 * dt;
        dphi_dt += d2phi_dt2 * dt;

        // Update position
        r += dr_dt * dt;
        phi += dphi_dt * dt;

        // Convert back to Cartesian coordinates
        pos.x = static_cast<float>(r * cos(phi) * VIS_SCALE);
        pos.y = static_cast<float>(r * sin(phi) * VIS_SCALE);

        // Update direction vector from new velocities
        Vector2 new_r_hat = {static_cast<float>(cos(phi)), static_cast<float>(sin(phi))};
        Vector2 new_phi_hat = {static_cast<float>(-sin(phi)), static_cast<float>(cos(phi))};

        // Convert polar velocities back to Cartesian direction
        Vector2 velocity_cart = {
            static_cast<float>(dr_dt * new_r_hat.x + r * dphi_dt * new_phi_hat.x),
            static_cast<float>(dr_dt * new_r_hat.y + r * dphi_dt * new_phi_hat.y)};

        dir = Vector2Normalize(velocity_cart);

        path.push_back(pos);
    }
};

struct Simulation
{
    BlackHole blackHole;
    std::vector<LightRay> lightRays;
    Vector2 center;

    Simulation(int width, int height)
        : blackHole(Vector2{0, 0}, 8.54e36), center{width / 2.0f, height / 2.0f}
    {
        // int numRays = 100;
        // int step = height / numRays;
        // for (int i = 0; i <= height; i += step)
        // {
        //     // Start rays from the left edge, relative to center
        //     LightRay ray(Vector2{-center.x, static_cast<float>(i) - center.y}, Vector2{1, 0});
        //     lightRays.push_back(ray);
        // }

        lightRays.emplace_back(Vector2{-center.x, 285.99}, Vector2{1, 0}); // Makes a single orbit around the black hole
    }

    void Update(double dt)
    {
        for (auto &lr : lightRays)
        {
            lr.Update(dt, blackHole.r_s); // Update each light ray's position
        }
    }

    void Draw()
    {
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw black hole at center
        float scaled_r_s = static_cast<float>(blackHole.r_s * VIS_SCALE);
        DrawCircleV(center, scaled_r_s, RED); // Draw the black hole as a circle with scaled radius

        // Draw light rays
        for (const auto &lr : lightRays)
        {
            DrawCircleV(Vector2Add(lr.pos, center), 2.0f, WHITE); // Draw the current position

            const size_t N = lr.path.size();
            for (size_t i = 0; i < lr.path.size() - 1; ++i)
            {
                float t = static_cast<float>(i) / (N - 1);
                Color fadeColor = {
                    static_cast<unsigned char>(255 * (t - 1.0f)),
                    static_cast<unsigned char>(255 * (t - 1.0f)),
                    static_cast<unsigned char>(255 * (t - 1.0f)),
                    255};
                DrawLineV(Vector2Add(lr.path[i], center), Vector2Add(lr.path[i + 1], center), fadeColor);
            }
        }

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
    Simulation sim(screenWidth, screenHeight);
    sim.Run();

    CloseWindow();
    return 0;
}
