#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VibeReaper {

    class Camera {
    public:
        Camera(float fov = 45.0f, float aspect = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 100.0f);

        // Get matrices
        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;

        // Camera controls
        void SetTarget(const glm::vec3& target);
        void Orbit(float deltaYaw, float deltaPitch);
        void Zoom(float delta);
        void Update(float deltaTime);

        // Setters
        void SetAspectRatio(float aspect);
        void SetDistance(float distValue);
        void SetRotation(float yaw, float pitch);

        // Getters
        glm::vec3 GetPosition() const { return position; }
        glm::vec3 GetTarget() const { return target; }
        float GetYaw() const { return yaw; }
        float GetPitch() const { return pitch; }
        float GetDistance() const { return distFromTarget; }

    private:
        // Camera parameters
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up;

        // Orbit parameters
        float yaw;     // Horizontal rotation
        float pitch;   // Vertical rotation
        float distFromTarget; // Distance from target

        // Projection parameters
        float fov;
        float aspectRatio;
        float nearPlane;
        float farPlane;

        // Update camera position based on orbit parameters
        void UpdatePosition();
    };

} // namespace VibeReaper
