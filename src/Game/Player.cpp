#include "Player.h"
#include "../Utils/Logger.h"
#include "../Engine/Constants.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

namespace VibeReaper {

    Player::Player()
        : position(0.0f, 0.0f, 0.0f),
          velocity(0.0f, 0.0f, 0.0f),
          yaw(0.0f),
          pitch(0.0f),
          moveSpeed(5.0_u),           // 5 m/s walking speed
          acceleration(0.15f),        // Time to reach full speed
          rotationSpeed(12.0f),       // Radians per second
          movementInput(0.0f, 0.0f),
          cameraYaw(0.0f),
          meshInitialized(false) {

        LOG_INFO("Player initialized with moveSpeed: " + std::to_string(moveSpeed / MAP_UNITS_PER_METER) +
                 " m/s (" + std::to_string(moveSpeed) + " MAP units/sec)");
    }

    void Player::ProcessInput(Input& input, Camera& camera, float deltaTime) {
        // Get movement input from WASD or gamepad left stick
        glm::vec2 inputDir(0.0f, 0.0f);

        // Keyboard input
        if (input.IsKeyPressed(SDL_SCANCODE_W)) inputDir.y += 1.0f;
        if (input.IsKeyPressed(SDL_SCANCODE_S)) inputDir.y -= 1.0f;
        if (input.IsKeyPressed(SDL_SCANCODE_A)) inputDir.x -= 1.0f;
        if (input.IsKeyPressed(SDL_SCANCODE_D)) inputDir.x += 1.0f;

        // Gamepad input (left stick)
        if (input.IsGamepadConnected()) {
            float stickX = input.GetAxis(SDL_CONTROLLER_AXIS_LEFTX);
            float stickY = -input.GetAxis(SDL_CONTROLLER_AXIS_LEFTY); // Invert Y

            if (std::abs(stickX) > 0.01f || std::abs(stickY) > 0.01f) {
                inputDir.x = stickX;
                inputDir.y = stickY;
            }
        }

        // Normalize diagonal movement to prevent faster diagonal speed
        if (glm::length(inputDir) > 1.0f) {
            inputDir = glm::normalize(inputDir);
        }

        // Store movement input and camera yaw for Update()
        movementInput = inputDir;
        cameraYaw = glm::radians(camera.GetYaw()); // Convert camera yaw to radians
    }

    void Player::Update(float deltaTime) {
        ApplyMovement(deltaTime);
        UpdateRotation(deltaTime);

        // Update position based on velocity
        position += velocity * deltaTime;

        // Keep player at ground level (no jumping/falling)
        position.y = 0.0f;
        velocity.y = 0.0f;
    }

    void Player::Render(Shader& shader) {
        if (!meshInitialized) {
            InitializeMesh();
        }

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(WIDTH * 0.5f, HEIGHT * 0.5f, WIDTH * 0.5f));

        shader.Use();
        shader.SetMat4("uModel", model);
        shader.SetVec3("uColor", glm::vec3(0.2f, 0.8f, 0.3f)); // Green player

        playerMesh.Draw(shader);
    }

    glm::vec3 Player::GetForward() const {
        return glm::vec3(std::sin(yaw), 0.0f, std::cos(yaw));
    }

    void Player::InitializeMesh() {
        // Create simple capsule/box mesh for player visualization
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Simple box for now (can be replaced with capsule later)
        float hw = 0.5f; // Half width
        float hh = 1.0f; // Half height
        float hd = 0.5f; // Half depth

        // Front face (+Z)
        vertices.push_back({{-hw, -hh, hd}, {0, 0, 1}, {0, 0}});
        vertices.push_back({{hw, -hh, hd}, {0, 0, 1}, {1, 0}});
        vertices.push_back({{hw, hh, hd}, {0, 0, 1}, {1, 1}});
        vertices.push_back({{-hw, hh, hd}, {0, 0, 1}, {0, 1}});

        // Back face (-Z)
        vertices.push_back({{hw, -hh, -hd}, {0, 0, -1}, {0, 0}});
        vertices.push_back({{-hw, -hh, -hd}, {0, 0, -1}, {1, 0}});
        vertices.push_back({{-hw, hh, -hd}, {0, 0, -1}, {1, 1}});
        vertices.push_back({{hw, hh, -hd}, {0, 0, -1}, {0, 1}});

        // Left face (-X)
        vertices.push_back({{-hw, -hh, -hd}, {-1, 0, 0}, {0, 0}});
        vertices.push_back({{-hw, -hh, hd}, {-1, 0, 0}, {1, 0}});
        vertices.push_back({{-hw, hh, hd}, {-1, 0, 0}, {1, 1}});
        vertices.push_back({{-hw, hh, -hd}, {-1, 0, 0}, {0, 1}});

        // Right face (+X)
        vertices.push_back({{hw, -hh, hd}, {1, 0, 0}, {0, 0}});
        vertices.push_back({{hw, -hh, -hd}, {1, 0, 0}, {1, 0}});
        vertices.push_back({{hw, hh, -hd}, {1, 0, 0}, {1, 1}});
        vertices.push_back({{hw, hh, hd}, {1, 0, 0}, {0, 1}});

        // Top face (+Y)
        vertices.push_back({{-hw, hh, hd}, {0, 1, 0}, {0, 0}});
        vertices.push_back({{hw, hh, hd}, {0, 1, 0}, {1, 0}});
        vertices.push_back({{hw, hh, -hd}, {0, 1, 0}, {1, 1}});
        vertices.push_back({{-hw, hh, -hd}, {0, 1, 0}, {0, 1}});

        // Bottom face (-Y)
        vertices.push_back({{-hw, -hh, -hd}, {0, -1, 0}, {0, 0}});
        vertices.push_back({{hw, -hh, -hd}, {0, -1, 0}, {1, 0}});
        vertices.push_back({{hw, -hh, hd}, {0, -1, 0}, {1, 1}});
        vertices.push_back({{-hw, -hh, hd}, {0, -1, 0}, {0, 1}});

        // Indices for each face (2 triangles per face)
        for (int i = 0; i < 6; ++i) {
            int base = i * 4;
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
        }

        playerMesh = Mesh(vertices, indices);
        playerMesh.SetupMesh(); // Initialize GPU buffers
        meshInitialized = true;

        LOG_INFO("Player mesh initialized");
    }

    void Player::ApplyMovement(float deltaTime) {
        if (glm::length(movementInput) > 0.01f) {
            // Calculate movement direction relative to camera
            // Camera looks down -Z axis, so we need to rotate input by camera yaw

            float inputX = movementInput.x;   // Left/Right (A/D)
            float inputZ = -movementInput.y;  // Forward/Back (W/S) - negated for correct direction

            // Rotate movement vector by camera yaw (negate for correct rotation direction)
            float cosYaw = std::cos(-cameraYaw);
            float sinYaw = std::sin(-cameraYaw);

            float worldX = inputX * cosYaw - inputZ * sinYaw;
            float worldZ = inputX * sinYaw + inputZ * cosYaw;

            // Calculate desired velocity
            glm::vec3 desiredVelocity(worldX * moveSpeed, velocity.y, worldZ * moveSpeed);

            // Lerp current velocity toward desired velocity (acceleration)
            float lerpFactor = 1.0f - std::pow(1.0f - acceleration, deltaTime * 60.0f);
            velocity.x = glm::mix(velocity.x, desiredVelocity.x, lerpFactor);
            velocity.z = glm::mix(velocity.z, desiredVelocity.z, lerpFactor);
        } else {
            // Deceleration when no input
            float lerpFactor = 1.0f - std::pow(1.0f - acceleration * 2.0f, deltaTime * 60.0f);
            velocity.x = glm::mix(velocity.x, 0.0f, lerpFactor);
            velocity.z = glm::mix(velocity.z, 0.0f, lerpFactor);
        }
    }


    void Player::UpdateRotation(float deltaTime) {
        // Rotate player to face movement direction
        if (glm::length(movementInput) > 0.01f) {
            // Calculate world-space movement direction
            float inputX = movementInput.x;
            float inputZ = -movementInput.y;

            float cosYaw = std::cos(-cameraYaw);
            float sinYaw = std::sin(-cameraYaw);

            float worldX = inputX * cosYaw - inputZ * sinYaw;
            float worldZ = inputX * sinYaw + inputZ * cosYaw;

            // Calculate target yaw from world-space direction
            float targetYaw = std::atan2(worldX, worldZ);

            // Smooth rotation toward target
            float angleDiff = targetYaw - yaw;

            // Normalize angle difference to [-PI, PI]
            while (angleDiff > glm::pi<float>()) angleDiff -= 2.0f * glm::pi<float>();
            while (angleDiff < -glm::pi<float>()) angleDiff += 2.0f * glm::pi<float>();

            float rotationStep = rotationSpeed * deltaTime;
            if (std::abs(angleDiff) < rotationStep) {
                yaw = targetYaw;
            } else {
                yaw += (angleDiff > 0.0f ? rotationStep : -rotationStep);
            }
        }
    }

} // namespace VibeReaper
