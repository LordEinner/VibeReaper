#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>

namespace VibeReaper {

    // Plane definition from MAP file
    struct Plane {
        glm::vec3 p1, p2, p3;           // Three points defining plane
        glm::vec3 normal;                // Computed normal
        float distance;                  // Distance from origin
        std::string texture;             // Texture name
        float offsetX, offsetY;          // Texture offset
        float rotation;                  // Texture rotation (degrees)
        float scaleX, scaleY;           // Texture scale

        Plane() : p1(0.0f), p2(0.0f), p3(0.0f), normal(0.0f), distance(0.0f),
                  offsetX(0.0f), offsetY(0.0f), rotation(0.0f), scaleX(1.0f), scaleY(1.0f) {}
    };

    // Brush definition (convex solid defined by planes)
    struct Brush {
        std::vector<Plane> planes;
    };

    // Entity definition (game object or worldspawn)
    struct Entity {
        std::string classname;
        std::map<std::string, std::string> properties;
        std::vector<Brush> brushes;     // Empty for point entities

        // Helper methods to extract typed properties
        glm::vec3 GetOrigin() const;
        int GetInt(const std::string& key, int defaultValue = 0) const;
        float GetFloat(const std::string& key, float defaultValue = 0.0f) const;
        std::string GetString(const std::string& key, const std::string& defaultValue = "") const;
        glm::vec3 GetVector3(const std::string& key, const glm::vec3& defaultValue = glm::vec3(0)) const;
    };

    // Complete MAP structure
    struct Map {
        std::vector<Entity> entities;
        Entity worldspawn;              // First entity (index 0)

        // Helper methods to query entities
        Entity* FindEntityByClass(const std::string& classname);
        std::vector<Entity*> FindEntitiesByClass(const std::string& classname);
    };

    // MAP file parser
    class MapLoader {
    public:
        // Load and parse a .map file
        static Map LoadFromFile(const std::string& path);

    private:
        // Parsing functions
        static std::vector<std::string> SplitIntoBlocks(const std::string& content, char open, char close);
        static Entity ParseEntity(const std::string& block);
        static Brush ParseBrush(const std::string& block);
        static Plane ParsePlane(const std::string& line);

        // Tokenization
        static std::vector<std::string> Tokenize(const std::string& line);
        static bool IsWhitespace(char c);

        // Helpers
        static void ComputePlaneEquation(Plane& plane);
        static std::string Trim(const std::string& str);
        static std::string RemoveComments(const std::string& content);
    };

} // namespace VibeReaper
