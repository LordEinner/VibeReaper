#include <iostream>
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Renderer.h"
#include "Engine/Shader.h"
#include "Engine/Mesh.h"
#include "Engine/Texture.h"
#include "Engine/Camera.h"
#include "Engine/Input.h"
#include "Engine/Constants.h"
#include "Utils/Logger.h"
#include "Game/World.h"
#include "Game/Player.h"

using namespace VibeReaper;

/**
 * SCALE SYSTEM DOCUMENTATION
 *
 * This project uses a configurable scale between Quake MAP units and real-world meters.
 * The conversion ratio is defined in Engine/Constants.h as MAP_UNITS_PER_METER.
 *
 * Current setting: 64 MAP units = 1 meter
 *
 * USER-DEFINED LITERAL: _u
 * Use the '_u' suffix to write measurements in meters that automatically convert to MAP units.
 *
 * Examples:
 * - Player height: 1.75_u    → 112.0 MAP units (1.75m × 64)
 * - Camera distance: 5.0_u   → 320.0 MAP units (5m × 64)
 * - Movement speed: 5.5_u    → 352.0 MAP units/sec (5.5 m/s × 64)
 *
 * This provides clean, readable code while maintaining the scale abstraction.
 * Changing MAP_UNITS_PER_METER automatically updates all _u measurements.
 *
 * This scale is applied throughout the engine for:
 * - Player dimensions (0.8m × 1.75m human size)
 * - Movement speeds (5 m/s walking speed)
 * - Camera distances (5m third-person offset)
 * - Physics values (gravity, jump strength, etc.)
 */

// Screen dimensions
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int main(int argc, char* argv[]) {
    // Initialize Logger
    LOG_INFO("Starting VibeReaper...");

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        LOG_ERROR("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
        return -1;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    SDL_Window* window = SDL_CreateWindow("VibeReaper - Phase 2",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == nullptr) {
        LOG_ERROR("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));
        SDL_Quit();
        return -1;
    }

    // Create OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr) {
        LOG_ERROR("OpenGL context could not be created! SDL_Error: " + std::string(SDL_GetError()));
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        LOG_ERROR("Failed to initialize GLAD");
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    LOG_INFO("OpenGL Version: " + std::string((char*)glGetString(GL_VERSION)));
    LOG_INFO("Renderer: " + std::string((char*)glGetString(GL_RENDERER)));

    // Initialize Renderer
    Renderer renderer;
    if (!renderer.Initialize()) {
        LOG_ERROR("Failed to initialize Renderer");
        return -1;
    }
    renderer.SetViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Load lighting shader
    Shader shader;
    if (!shader.LoadFromFiles("assets/shaders/lighting.vert", "assets/shaders/lighting.frag")) {
        LOG_ERROR("Failed to load lighting shaders");
        return -1;
    }

    // Load texture
    Texture texture;
    if (!texture.LoadFromFile("assets/textures/test_texture.png")) {
        LOG_WARNING("Failed to load test texture, creating fallback white texture");
        texture.CreateWhiteTexture();
    }

    // Load world from MAP file
    World world;
    if (!world.LoadMap("assets/maps/debug_test.map")) {
        LOG_ERROR("Failed to load map, exiting");
        return -1;
    }

    // Get player spawn position
    glm::vec3 playerSpawn = world.GetPlayerSpawnPosition();
    // Transform spawn position from Z-up to Y-up: (x, y, z) -> (x, z, -y)
    glm::vec3 engineSpawn(playerSpawn.x, playerSpawn.z, -playerSpawn.y);

    LOG_INFO("Player spawn (Engine): " + std::to_string(engineSpawn.x) + ", " +
             std::to_string(engineSpawn.y) + ", " + std::to_string(engineSpawn.z));

    // Create player at spawn position
    Player player;
    player.SetPosition(engineSpawn);

    // Create input system
    Input input;
    input.SetMouseCaptured(true); // Capture mouse for camera control

    // Get light position from map (assuming first light found)
    glm::vec3 lightPos(0.0f, 500.0f, 0.0f); // Default high above
    std::vector<const Entity*> lights = world.GetEntitiesByClass("light");
    if (!lights.empty()) {
        glm::vec3 mapLightPos = lights[0]->GetOrigin();
        // Transform light position from Z-up to Y-up
        lightPos = glm::vec3(mapLightPos.x, mapLightPos.z, -mapLightPos.y);
        LOG_INFO("Light spawn (Quake): " + std::to_string(mapLightPos.x) + ", " + 
                 std::to_string(mapLightPos.y) + ", " + std::to_string(mapLightPos.z));
        LOG_INFO("Light spawn (Engine): " + std::to_string(lightPos.x) + ", " + 
                 std::to_string(lightPos.y) + ", " + std::to_string(lightPos.z));
    } else {
        LOG_INFO("No light found in map, using default at (0, 500, 0)");
    }

    // Create camera
    float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    Camera camera(45.0f, aspectRatio, 0.1f, 4000.0f);

    // Set camera to orbit player (third-person)
    // Target player's center (half of player height: 1.75m / 2 = 0.875m)
    const float playerCenterHeight = 0.875_u;  // 1.75m / 2
    glm::vec3 playerCenter = engineSpawn + glm::vec3(0.0f, playerCenterHeight, 0.0f);
    camera.SetTarget(playerCenter);
    camera.SetDistance(5.0_u);  // 5 meters behind player
    camera.SetRotation(0.0f, 20.0f); // Behind player, slightly elevated

    // Lighting parameters
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    // Main loop
    bool quit = false;
    SDL_Event e;

    // Time management
    Uint64 lastTime = SDL_GetPerformanceCounter();
    double deltaTime = 0.0;

    // Game Loop
    while (!quit) {
        // Calculate Delta Time
        Uint64 currentTime = SDL_GetPerformanceCounter();
        deltaTime = (double)((currentTime - lastTime) * 1000 / (double)SDL_GetPerformanceFrequency());
        deltaTime /= 1000.0; // Convert to seconds
        lastTime = currentTime;

        // FPS Counter
        static float fpsTimer = 0.0f;
        static int frameCount = 0;
        fpsTimer += deltaTime;
        frameCount++;

        if (fpsTimer >= 1.0f) {
            float fps = frameCount / fpsTimer;
            LOG_INFO("FPS: " + std::to_string((int)fps));
            fpsTimer = 0.0f;
            frameCount = 0;
        }

        // Handle Events
        while (SDL_PollEvent(&e) != 0) {
            // Process input events
            input.ProcessEvent(e);

            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int width = e.window.data1;
                    int height = e.window.data2;
                    renderer.SetViewport(0, 0, width, height);
                    camera.SetAspectRatio((float)width / (float)height);
                }
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
            }
        }

        // Update input state
        input.Update();

        // Process player input
        player.ProcessInput(input, camera, deltaTime);

        // Update player physics
        player.Update(deltaTime);

        // Camera rotation via mouse
        glm::vec2 mouseDelta = input.GetMouseDelta();
        if (glm::length(mouseDelta) > 0.01f) {
            float horizontalMult = input.GetInvertHorizontal() ? -1.0f : 1.0f;
            float verticalMult = input.GetInvertVertical() ? -1.0f : 1.0f;
            camera.Orbit(mouseDelta.x * 0.15f * horizontalMult, mouseDelta.y * 0.15f * verticalMult);
        }

        // Camera rotation via gamepad right stick
        if (input.IsGamepadConnected()) {
            float rightX = input.GetAxis(SDL_CONTROLLER_AXIS_RIGHTX);
            float rightY = input.GetAxis(SDL_CONTROLLER_AXIS_RIGHTY);
            if (std::abs(rightX) > 0.01f || std::abs(rightY) > 0.01f) {
                float horizontalMult = input.GetInvertHorizontal() ? -1.0f : 1.0f;
                float verticalMult = input.GetInvertVertical() ? -1.0f : 1.0f;
                camera.Orbit(rightX * 3.0f * horizontalMult, rightY * 3.0f * verticalMult);
            }
        }

        // Camera zoom
        if (input.IsKeyPressed(SDL_SCANCODE_EQUALS)) {
            camera.Zoom(5.0f * deltaTime);
        }
        if (input.IsKeyPressed(SDL_SCANCODE_MINUS)) {
            camera.Zoom(-5.0f * deltaTime);
        }

        // Camera follow player center (half of player height: 1.75m / 2 = 0.875m)
        const float playerCenterHeight = 0.875_u;  // 1.75m / 2
        glm::vec3 playerCenter = player.GetPosition() + glm::vec3(0.0f, playerCenterHeight, 0.0f);
        camera.FollowTargetWithCollision(playerCenter, &world, deltaTime);
        camera.Update(deltaTime);

        // Render
        renderer.Clear();

        // Use lighting shader
        shader.Use();

        // Set uniforms
        shader.SetMat4("uView", camera.GetViewMatrix());
        shader.SetMat4("uProjection", camera.GetProjectionMatrix());

        shader.SetVec3("uLightPos", lightPos);
        shader.SetVec3("uLightColor", lightColor);
        shader.SetVec3("uViewPos", camera.GetPosition());
        shader.SetFloat("uAmbientStrength", 0.5f);
        shader.SetFloat("uSpecularStrength", 0.5f);
        shader.SetFloat("uShininess", 32.0f);

        // Set texture sampler to use texture unit 0
        shader.SetInt("uTexture", 0);
        
        shader.SetVec3("uColor", glm::vec3(1.0f, 1.0f, 1.0f));

        // Render world with rotation (Quake Z-up to Engine Y-up)
        glm::mat4 worldModel = glm::mat4(1.0f);
        worldModel = glm::rotate(worldModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        shader.SetMat4("uModel", worldModel);

        // World handles texture binding now
        world.Render(shader);

        // Render player (no world rotation needed - player is already in engine space)
        player.Render(shader);

        // Swap buffers
        renderer.SwapBuffers(window);
    }

    // Cleanup
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    LOG_INFO("VibeReaper shutdown successfully");
    return 0;
}
