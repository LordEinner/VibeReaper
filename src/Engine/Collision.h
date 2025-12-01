#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace VibeReaper {

    /**
     * @brief Axis-Aligned Bounding Box for collision detection
     */
    struct AABB {
        glm::vec3 min;
        glm::vec3 max;

        AABB() : min(0.0f), max(0.0f) {}
        AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

        /**
         * @brief Check if this AABB intersects another
         */
        bool Intersects(const AABB& other) const {
            return (min.x <= other.max.x && max.x >= other.min.x) &&
                   (min.y <= other.max.y && max.y >= other.min.y) &&
                   (min.z <= other.max.z && max.z >= other.min.z);
        }

        /**
         * @brief Get center point of AABB
         */
        glm::vec3 GetCenter() const {
            return (min + max) * 0.5f;
        }

        /**
         * @brief Get size (extents) of AABB
         */
        glm::vec3 GetSize() const {
            return max - min;
        }

        /**
         * @brief Expand AABB to include a point
         */
        void Expand(const glm::vec3& point) {
            min = glm::min(min, point);
            max = glm::max(max, point);
        }

        /**
         * @brief Create AABB from center and half-extents
         */
        static AABB FromCenterAndExtents(const glm::vec3& center, const glm::vec3& extents) {
            return AABB(center - extents, center + extents);
        }
    };

    /**
     * @brief Collision resolution result
     */
    struct CollisionResult {
        bool hit;
        glm::vec3 normal;      // Surface normal
        float penetration;     // Penetration depth
        glm::vec3 contactPoint; // Point of contact

        CollisionResult()
            : hit(false), normal(0.0f), penetration(0.0f), contactPoint(0.0f) {}
    };

    /**
     * @brief Static collision detection utilities
     */
    class Collision {
    public:
        /**
         * @brief Test AABB vs AABB collision
         */
        static bool TestAABB(const AABB& a, const AABB& b);

        /**
         * @brief Resolve AABB collision with minimum translation vector
         * @param moving The moving AABB
         * @param stationary The stationary AABB
         * @return Collision result with normal and penetration depth
         */
        static CollisionResult ResolveAABB(const AABB& moving, const AABB& stationary);

        /**
         * @brief Raycast against AABB
         * @param origin Ray origin
         * @param direction Ray direction (normalized)
         * @param aabb Target AABB
         * @param maxDistance Maximum ray distance
         * @return Collision result with hit distance in penetration field
         */
        static CollisionResult RaycastAABB(const glm::vec3& origin,
                                           const glm::vec3& direction,
                                           const AABB& aabb,
                                           float maxDistance = 1000.0f);

        /**
         * @brief Slide velocity along collision normal
         * @param velocity Current velocity
         * @param normal Surface normal
         * @return Adjusted velocity parallel to surface
         */
        static glm::vec3 SlideVelocity(const glm::vec3& velocity, const glm::vec3& normal);
    };

} // namespace VibeReaper
