#include "Camera.h"
#include "../Utils/Logger.h"

namespace VibeReaper {

    Camera::Camera(float fov, float aspect, float nearPlane, float farPlane)
        : fov(fov), aspectRatio(aspect), nearPlane(nearPlane), farPlane(farPlane),
          target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f),
          yaw(0.0f), pitch(20.0f), distFromTarget(5.0f) {
        UpdatePosition();
        LOG_INFO("Camera initialized (FOV: " + std::to_string(fov) + "°, aspect: " + 
                 std::to_string(aspect) + ")");
    }

    glm::mat4 Camera::GetViewMatrix() const {
        return glm::lookAt(position, target, up);
    }

    glm::mat4 Camera::GetProjectionMatrix() const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    void Camera::SetTarget(const glm::vec3& newTarget) {
        target = newTarget;
        UpdatePosition();
    }

    void Camera::Orbit(float deltaYaw, float deltaPitch) {
        yaw += deltaYaw;
        pitch += deltaPitch;

        // Clamp pitch to avoid gimbal lock
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        UpdatePosition();
    }

    void Camera::Zoom(float delta) {
        distFromTarget -= delta;

        // Clamp distance
        if (distFromTarget < 1.0f) distFromTarget = 1.0f;
        if (distFromTarget > 50.0f) distFromTarget = 50.0f;

        UpdatePosition();
    }

    void Camera::Update(float deltaTime) {
        // Can add smooth interpolation or momentum here if needed
        // For now, position is updated directly in Orbit/Zoom
    }

    void Camera::SetAspectRatio(float aspect) {
        aspectRatio = aspect;
    }

    void Camera::SetDistance(float distValue) {
        distFromTarget = distValue;
        UpdatePosition();
    }

    void Camera::UpdatePosition() {
        // Convert spherical coordinates (yaw, pitch, distance) to Cartesian
        float yawRad = glm::radians(yaw);
        float pitchRad = glm::radians(pitch);

        float x = distFromTarget * cos(pitchRad) * sin(yawRad);
        float y = distFromTarget * sin(pitchRad);
        float z = distFromTarget * cos(pitchRad) * cos(yawRad);

        position = target + glm::vec3(x, y, z);
    }

} // namespace VibeReaper
