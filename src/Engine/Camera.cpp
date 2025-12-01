#include "Camera.h"
#include "../Utils/Logger.h"
#include "../Game/World.h"
#include "Collision.h"
#include "Constants.h"
#include <cfloat>

using namespace VibeReaper;

namespace VibeReaper {

    Camera::Camera(float fov, float aspect, float nearPlane, float farPlane)
        : fov(fov), aspectRatio(aspect), nearPlane(nearPlane), farPlane(farPlane),
          target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f),
          yaw(0.0f), pitch(20.0f), distFromTarget(5.0f),
          desiredDistance(5.0f), currentDistance(5.0f) {
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

    void Camera::FollowTarget(const glm::vec3& targetPos, float deltaTime) {
        // Smoothly lerp target position toward player position
        const float lerpFactor = 0.1f;
        float smoothFactor = 1.0f - pow(1.0f - lerpFactor, deltaTime * 60.0f);

        target = glm::mix(target, targetPos, smoothFactor);
        UpdatePosition();
    }

    void Camera::FollowTargetWithCollision(const glm::vec3& targetPos, const World* world, float deltaTime) {
        // Smoothly lerp target position toward player position
        const float lerpFactor = 0.1f;
        float smoothFactor = 1.0f - pow(1.0f - lerpFactor, deltaTime * 60.0f);

        target = glm::mix(target, targetPos, smoothFactor);

        // Store desired distance
        desiredDistance = distFromTarget;

        // Calculate ideal camera position
        float yawRad = glm::radians(yaw);
        float pitchRad = glm::radians(pitch);

        float x = desiredDistance * cos(pitchRad) * sin(yawRad);
        float y = desiredDistance * sin(pitchRad);
        float z = desiredDistance * cos(pitchRad) * cos(yawRad);

        glm::vec3 idealPosition = target + glm::vec3(x, y, z);

        // Raycast from target to ideal camera position
        if (world) {
            glm::vec3 direction = glm::normalize(idealPosition - target);
            float rayDistance = desiredDistance;

            // Check collision with world geometry
            // Cast a ray from player toward camera, check each mesh AABB

            float minDistance = rayDistance;
            const float cameraRadius = 0.5_u; // Small radius to avoid clipping (0.5m)

            const auto& geometry = world->GetLevelGeometry();
            for (const auto& renderObj : geometry) {
                // Calculate AABB from mesh vertices
                if (renderObj.mesh.vertices.empty()) continue;

                glm::vec3 bmin(FLT_MAX), bmax(-FLT_MAX);
                for (const auto& vertex : renderObj.mesh.vertices) {
                    bmin = glm::min(bmin, vertex.position);
                    bmax = glm::max(bmax, vertex.position);
                }

                AABB meshAABB{bmin, bmax};

                // Raycast against mesh AABB
                CollisionResult hitResult = Collision::RaycastAABB(target, direction, meshAABB, desiredDistance);
                if (hitResult.hit) {
                    float hitDistance = hitResult.penetration; // penetration stores the ray distance (t value)
                    if (hitDistance > 0.0f && hitDistance < minDistance) {
                        minDistance = hitDistance - cameraRadius; // Pull back by camera radius
                        if (minDistance < 0.5_u) minDistance = 0.5_u; // Minimum distance (0.5m)
                    }
                }
            }

            // Smoothly interpolate current distance toward collision distance
            const float distanceLerpFactor = 0.3f;
            float distanceSmoothFactor = 1.0f - pow(1.0f - distanceLerpFactor, deltaTime * 60.0f);
            currentDistance = glm::mix(currentDistance, minDistance, distanceSmoothFactor);

            // Update position with collision-adjusted distance
            x = currentDistance * cos(pitchRad) * sin(yawRad);
            y = currentDistance * sin(pitchRad);
            z = currentDistance * cos(pitchRad) * cos(yawRad);

            position = target + glm::vec3(x, y, z);
        } else {
            // No world collision, use standard update
            UpdatePosition();
        }
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
        desiredDistance = distValue;
        currentDistance = distValue;
        UpdatePosition();
    }

    void Camera::SetRotation(float newYaw, float newPitch) {
        yaw = newYaw;
        pitch = newPitch;
        
        // Clamp pitch
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

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
