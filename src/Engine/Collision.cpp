#include "Collision.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace VibeReaper {

    bool Collision::TestAABB(const AABB& a, const AABB& b) {
        return a.Intersects(b);
    }

    CollisionResult Collision::ResolveAABB(const AABB& moving, const AABB& stationary) {
        CollisionResult result;

        if (!moving.Intersects(stationary)) {
            result.hit = false;
            return result;
        }

        result.hit = true;

        // Calculate overlap on each axis
        float overlapX = std::min(moving.max.x, stationary.max.x) -
                        std::max(moving.min.x, stationary.min.x);
        float overlapY = std::min(moving.max.y, stationary.max.y) -
                        std::max(moving.min.y, stationary.min.y);
        float overlapZ = std::min(moving.max.z, stationary.max.z) -
                        std::max(moving.min.z, stationary.min.z);

        // Find minimum overlap axis (axis of least penetration)
        if (overlapX < overlapY && overlapX < overlapZ) {
            // X-axis separation
            result.penetration = overlapX;
            result.normal = moving.GetCenter().x < stationary.GetCenter().x ?
                           glm::vec3(-1.0f, 0.0f, 0.0f) :
                           glm::vec3(1.0f, 0.0f, 0.0f);
        } else if (overlapY < overlapZ) {
            // Y-axis separation
            result.penetration = overlapY;
            result.normal = moving.GetCenter().y < stationary.GetCenter().y ?
                           glm::vec3(0.0f, -1.0f, 0.0f) :
                           glm::vec3(0.0f, 1.0f, 0.0f);
        } else {
            // Z-axis separation
            result.penetration = overlapZ;
            result.normal = moving.GetCenter().z < stationary.GetCenter().z ?
                           glm::vec3(0.0f, 0.0f, -1.0f) :
                           glm::vec3(0.0f, 0.0f, 1.0f);
        }

        // Calculate contact point (center of overlap region)
        glm::vec3 overlapMin = glm::max(moving.min, stationary.min);
        glm::vec3 overlapMax = glm::min(moving.max, stationary.max);
        result.contactPoint = (overlapMin + overlapMax) * 0.5f;

        return result;
    }

    CollisionResult Collision::RaycastAABB(const glm::vec3& origin,
                                           const glm::vec3& direction,
                                           const AABB& aabb,
                                           float maxDistance) {
        CollisionResult result;
        result.hit = false;

        // Slab method for ray-AABB intersection
        float tMin = 0.0f;
        float tMax = maxDistance;

        for (int i = 0; i < 3; ++i) {
            if (std::abs(direction[i]) < 1e-6f) {
                // Ray parallel to slab
                if (origin[i] < aabb.min[i] || origin[i] > aabb.max[i]) {
                    return result; // No intersection
                }
            } else {
                // Compute intersection with near and far planes
                float invD = 1.0f / direction[i];
                float t1 = (aabb.min[i] - origin[i]) * invD;
                float t2 = (aabb.max[i] - origin[i]) * invD;

                if (t1 > t2) std::swap(t1, t2);

                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);

                if (tMin > tMax) {
                    return result; // No intersection
                }
            }
        }

        result.hit = true;
        result.penetration = tMin; // Distance to intersection
        result.contactPoint = origin + direction * tMin;

        // Calculate normal (which face was hit)
        glm::vec3 hitPoint = result.contactPoint;
        glm::vec3 center = aabb.GetCenter();
        glm::vec3 extents = aabb.GetSize() * 0.5f;

        // Find closest face
        float minDist = std::numeric_limits<float>::max();
        glm::vec3 normal(0.0f);

        // Check each face
        float dist;
        dist = std::abs(hitPoint.x - aabb.min.x);
        if (dist < minDist) { minDist = dist; normal = glm::vec3(-1, 0, 0); }
        dist = std::abs(hitPoint.x - aabb.max.x);
        if (dist < minDist) { minDist = dist; normal = glm::vec3(1, 0, 0); }
        dist = std::abs(hitPoint.y - aabb.min.y);
        if (dist < minDist) { minDist = dist; normal = glm::vec3(0, -1, 0); }
        dist = std::abs(hitPoint.y - aabb.max.y);
        if (dist < minDist) { minDist = dist; normal = glm::vec3(0, 1, 0); }
        dist = std::abs(hitPoint.z - aabb.min.z);
        if (dist < minDist) { minDist = dist; normal = glm::vec3(0, 0, -1); }
        dist = std::abs(hitPoint.z - aabb.max.z);
        if (dist < minDist) { minDist = dist; normal = glm::vec3(0, 0, 1); }

        result.normal = normal;
        return result;
    }

    glm::vec3 Collision::SlideVelocity(const glm::vec3& velocity, const glm::vec3& normal) {
        // Project velocity onto plane defined by normal
        return velocity - normal * glm::dot(velocity, normal);
    }

} // namespace VibeReaper
