#pragma once

#include <glm/glm.hpp>
#include "../Engine/Input.h"
#include "../Engine/Camera.h"
#include "../Engine/Shader.h"
#include "../Engine/Mesh.h"
#include "../Engine/Constants.h"

namespace VibeReaper {

    /**
     * @brief Player character with movement, rotation, and physics
     *
     * Features:
     * - WASD / gamepad stick movement
     * - Camera-relative movement direction
     * - Smooth rotation to face movement direction
     * - Gravity and jumping
     * - Ground detection
     */
    class Player {
    public:
        Player();
        ~Player() = default;

        /**
         * @brief Process input and update movement direction
         * @param input Input system reference
         * @param camera Camera for movement direction calculation
         * @param deltaTime Frame time in seconds
         */
        void ProcessInput(Input& input, Camera& camera, float deltaTime);

        /**
         * @brief Update physics and position
         * @param deltaTime Frame time in seconds
         */
        void Update(float deltaTime);

        /**
         * @brief Render player model
         * @param shader Shader to use for rendering
         */
        void Render(Shader& shader);

        // Getters
        glm::vec3 GetPosition() const { return position; }
        glm::vec3 GetVelocity() const { return velocity; }
        float GetYaw() const { return yaw; }
        glm::vec3 GetForward() const;

        // Setters
        void SetPosition(const glm::vec3& pos) { position = pos; }

        // Player dimensions (for collision)
        // Human size: 0.8m × 1.75m (using _u literal for elegant meter-to-MAP-unit conversion)
        static constexpr float WIDTH = 0.8_u;
        static constexpr float HEIGHT = 1.75_u;

    private:
        // Transform
        glm::vec3 position;
        glm::vec3 velocity;
        float yaw;   // Horizontal rotation (radians)
        float pitch; // Vertical look angle (for future animation)

        // Movement parameters
        float moveSpeed;          // Units per second
        float acceleration;       // Time to reach full speed
        float rotationSpeed;      // Radians per second

        // Input state
        glm::vec2 movementInput;  // Normalized movement direction
        float cameraYaw;          // Camera yaw for movement direction

        // Rendering
        Mesh playerMesh;
        bool meshInitialized;

        // Helpers
        void InitializeMesh();
        void ApplyMovement(float deltaTime);
        void UpdateRotation(float deltaTime);
    };

} // namespace VibeReaper
