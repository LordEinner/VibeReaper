#pragma once

#include "MapLoader.h"
#include "Mesh.h"
#include <vector>

namespace VibeReaper {

    // Converts CSG brushes to triangle meshes
    class BrushConverter {
    public:
        // Convert a single brush to a mesh
        static Mesh ConvertBrushToMesh(const Brush& brush);

        // Convert multiple brushes to meshes
        static std::vector<Mesh> ConvertBrushesToMeshes(const std::vector<Brush>& brushes);

    private:
        // Vertex calculation
        static std::vector<glm::vec3> CalculateVertices(const std::vector<Plane>& planes);
        static glm::vec3 IntersectThreePlanes(const Plane& p1, const Plane& p2, const Plane& p3);
        static bool IsPointInsideBrush(const glm::vec3& point, const std::vector<Plane>& planes, float epsilon = 0.01f);

        // Face building
        static std::vector<Vertex> BuildFaces(const std::vector<Plane>& planes, const std::vector<glm::vec3>& vertices);
        static std::vector<Vertex> BuildFace(const Plane& plane, const std::vector<glm::vec3>& vertices);

        // Geometry helpers
        static void SortWindingOrder(std::vector<glm::vec3>& faceVertices, const glm::vec3& normal);
        static glm::vec2 CalculateUV(const glm::vec3& vertex, const Plane& plane);
        static std::vector<unsigned int> TriangulateFace(unsigned int startIndex, unsigned int vertexCount);
    };

} // namespace VibeReaper
