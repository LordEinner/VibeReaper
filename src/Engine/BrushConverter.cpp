#include "BrushConverter.h"
#include "../Utils/Logger.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <algorithm>
#include <cmath>

namespace VibeReaper {

    Mesh BrushConverter::ConvertBrushToMesh(const Brush& brush) {
        if (brush.planes.size() < 4) {
            LOG_WARNING("Brush has less than 4 planes, cannot form a 3D solid");
            return Mesh();
        }

        // Step 1: Calculate all vertices
        std::vector<glm::vec3> vertices = CalculateVertices(brush.planes);

        if (vertices.empty()) {
            LOG_WARNING("Brush generated no vertices");
            return Mesh();
        }

        LOG_INFO("Brush has " + std::to_string(vertices.size()) + " vertices");

        // Step 2: Build faces
        std::vector<Vertex> meshVertices = BuildFaces(brush.planes, vertices);

        if (meshVertices.empty()) {
            LOG_WARNING("Brush generated no faces");
            return Mesh();
        }

        // Step 3: Create indices (simple sequential since we're using triangle lists)
        std::vector<unsigned int> indices;
        for (unsigned int i = 0; i < meshVertices.size(); i++) {
            indices.push_back(i);
        }

        LOG_INFO("Generated mesh with " + std::to_string(meshVertices.size()) + " vertices and " + 
                 std::to_string(indices.size() / 3) + " triangles");

        return Mesh(meshVertices, indices);
    }

    std::vector<Mesh> BrushConverter::ConvertBrushesToMeshes(const std::vector<Brush>& brushes) {
        std::vector<Mesh> meshes;
        meshes.reserve(brushes.size());

        for (const auto& brush : brushes) {
            Mesh mesh = ConvertBrushToMesh(brush);
            if (!mesh.vertices.empty()) {
                meshes.push_back(std::move(mesh));
            }
        }

        return meshes;
    }

    std::vector<glm::vec3> BrushConverter::CalculateVertices(const std::vector<Plane>& planes) {
        std::vector<glm::vec3> vertices;
        int n = static_cast<int>(planes.size());

        // Test all combinations of 3 planes
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                for (int k = j + 1; k < n; k++) {
                    glm::vec3 vertex = IntersectThreePlanes(planes[i], planes[j], planes[k]);

                    // Check if intersection is valid (not NaN or inf)
                    if (std::isnan(vertex.x) || std::isnan(vertex.y) || std::isnan(vertex.z) ||
                        std::isinf(vertex.x) || std::isinf(vertex.y) || std::isinf(vertex.z)) {
                        continue;
                    }

                    // Check if vertex is inside brush (on correct side of all planes)
                    if (IsPointInsideBrush(vertex, planes)) {
                        // Check for duplicates
                        bool isDuplicate = false;
                        for (const auto& existing : vertices) {
                            if (glm::length(vertex - existing) < 0.01f) {
                                isDuplicate = true;
                                break;
                            }
                        }

                        if (!isDuplicate) {
                            vertices.push_back(vertex);
                        }
                    }
                }
            }
        }

        return vertices;
    }

    glm::vec3 BrushConverter::IntersectThreePlanes(const Plane& p1, const Plane& p2, const Plane& p3) {
        // Solve system of equations:
        // dot(n1, p) = d1
        // dot(n2, p) = d2
        // dot(n3, p) = d3

        glm::mat3 M(p1.normal, p2.normal, p3.normal);
        M = glm::transpose(M); // GLM stores column-major

        float det = glm::determinant(M);
        if (std::abs(det) < 0.0001f) {
            // Planes are parallel or degenerate
            return glm::vec3(std::numeric_limits<float>::quiet_NaN());
        }

        glm::vec3 d(p1.distance, p2.distance, p3.distance);
        return glm::inverse(M) * d;
    }

    bool BrushConverter::IsPointInsideBrush(const glm::vec3& point, const std::vector<Plane>& planes, float epsilon) {
        for (const auto& plane : planes) {
            float dist = glm::dot(plane.normal, point) - plane.distance;
            if (dist > epsilon) {
                return false; // Point is outside this plane
            }
        }
        return true;
    }

    std::vector<Vertex> BrushConverter::BuildFaces(const std::vector<Plane>& planes, const std::vector<glm::vec3>& vertices) {
        std::vector<Vertex> allVertices;

        // Build a face for each plane
        for (const auto& plane : planes) {
            std::vector<Vertex> faceVertices = BuildFace(plane, vertices);
            
            if (faceVertices.size() >= 3) {
                // Triangulate the face (fan triangulation from first vertex)
                for (size_t i = 1; i < faceVertices.size() - 1; i++) {
                    allVertices.push_back(faceVertices[0]);
                    allVertices.push_back(faceVertices[i]);
                    allVertices.push_back(faceVertices[i + 1]);
                }
            }
        }

        return allVertices;
    }

    std::vector<Vertex> BrushConverter::BuildFace(const Plane& plane, const std::vector<glm::vec3>& vertices) {
        // Find all vertices that lie on this plane
        std::vector<glm::vec3> faceVertices;

        for (const auto& vertex : vertices) {
            float dist = std::abs(glm::dot(plane.normal, vertex) - plane.distance);
            if (dist < 0.01f) {
                faceVertices.push_back(vertex);
            }
        }

        if (faceVertices.size() < 3) {
            return {}; // Degenerate face
        }

        // Sort vertices in winding order
        SortWindingOrder(faceVertices, plane.normal);

        // Create Vertex structures with UVs
        std::vector<Vertex> result;
        for (const auto& pos : faceVertices) {
            Vertex v;
            v.position = pos;
            v.normal = plane.normal;
            v.texCoord = CalculateUV(pos, plane);
            result.push_back(v);
        }

        return result;
    }

    void BrushConverter::SortWindingOrder(std::vector<glm::vec3>& faceVertices, const glm::vec3& normal) {
        if (faceVertices.size() < 3) return;

        // Calculate centroid
        glm::vec3 center(0.0f);
        for (const auto& v : faceVertices) {
            center += v;
        }
        center /= static_cast<float>(faceVertices.size());

        // Create basis vectors for 2D projection
        glm::vec3 up = std::abs(normal.y) < 0.9f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
        glm::vec3 right = glm::normalize(glm::cross(up, normal));
        up = glm::cross(normal, right);

        // Sort by angle around center
        std::sort(faceVertices.begin(), faceVertices.end(), [&](const glm::vec3& a, const glm::vec3& b) {
            glm::vec3 da = a - center;
            glm::vec3 db = b - center;

            float angleA = std::atan2(glm::dot(da, up), glm::dot(da, right));
            float angleB = std::atan2(glm::dot(db, up), glm::dot(db, right));

            return angleA < angleB;
        });
    }

    glm::vec2 BrushConverter::CalculateUV(const glm::vec3& vertex, const Plane& plane) {
        // Simple planar projection
        // Create texture axes perpendicular to normal

        glm::vec3 up = std::abs(plane.normal.y) < 0.9f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
        glm::vec3 uAxis = glm::normalize(glm::cross(up, plane.normal));
        glm::vec3 vAxis = glm::cross(plane.normal, uAxis);

        // Project vertex onto texture axes
        // In Quake, scale represents "world units per texture pixel"
        // So we need to MULTIPLY (not divide) to get proper tiling
        float u = glm::dot(vertex, uAxis) * plane.scaleX;
        float v = glm::dot(vertex, vAxis) * plane.scaleY;

        // Apply rotation (convert degrees to radians, negate for Quake convention)
        float rotationRad = glm::radians(-plane.rotation);
        float cosRot = std::cos(rotationRad);
        float sinRot = std::sin(rotationRad);
        
        // Rotate UV coordinates around origin
        float uRotated = u * cosRot - v * sinRot;
        float vRotated = u * sinRot + v * cosRot;
        
        // Apply offset
        uRotated += plane.offsetX;
        vRotated += plane.offsetY;

        // Normalize to texture coordinates (assuming 64x64 default texture size from Quake)
        uRotated /= 64.0f;
        vRotated /= 64.0f;

        return glm::vec2(uRotated, vRotated);
    }

    std::vector<unsigned int> BrushConverter::TriangulateFace(unsigned int startIndex, unsigned int vertexCount) {
        std::vector<unsigned int> indices;

        // Fan triangulation
        for (unsigned int i = 1; i < vertexCount - 1; i++) {
            indices.push_back(startIndex);
            indices.push_back(startIndex + i);
            indices.push_back(startIndex + i + 1);
        }

        return indices;
    }

} // namespace VibeReaper
