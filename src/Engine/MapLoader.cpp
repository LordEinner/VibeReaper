#include "MapLoader.h"
#include "../Utils/Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace VibeReaper {

    // ========== Entity Helper Methods ==========

    glm::vec3 Entity::GetOrigin() const {
        return GetVector3("origin", glm::vec3(0.0f));
    }

    int Entity::GetInt(const std::string& key, int defaultValue) const {
        auto it = properties.find(key);
        if (it != properties.end()) {
            try {
                return std::stoi(it->second);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }

    float Entity::GetFloat(const std::string& key, float defaultValue) const {
        auto it = properties.find(key);
        if (it != properties.end()) {
            try {
                return std::stof(it->second);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }

    std::string Entity::GetString(const std::string& key, const std::string& defaultValue) const {
        auto it = properties.find(key);
        if (it != properties.end()) {
            return it->second;
        }
        return defaultValue;
    }

    glm::vec3 Entity::GetVector3(const std::string& key, const glm::vec3& defaultValue) const {
        auto it = properties.find(key);
        if (it != properties.end()) {
            // Parse "x y z" format
            std::istringstream iss(it->second);
            glm::vec3 result;
            if (iss >> result.x >> result.y >> result.z) {
                return result;
            }
        }
        return defaultValue;
    }

    // ========== Map Helper Methods ==========

    Entity* Map::FindEntityByClass(const std::string& classname) {
        for (auto& entity : entities) {
            if (entity.classname == classname) {
                return &entity;
            }
        }
        return nullptr;
    }

    std::vector<Entity*> Map::FindEntitiesByClass(const std::string& classname) {
        std::vector<Entity*> result;
        for (auto& entity : entities) {
            if (entity.classname == classname) {
                result.push_back(&entity);
            }
        }
        return result;
    }

    // ========== MapLoader Implementation ==========

    Map MapLoader::LoadFromFile(const std::string& path) {
        LOG_INFO("Loading MAP file: " + path);

        Map map;

        // Read entire file
        std::ifstream file(path);
        if (!file.is_open()) {
            LOG_ERROR("Failed to open MAP file: " + path);
            return map;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();

        // Remove comments
        content = RemoveComments(content);

        // Split into entity blocks
        std::vector<std::string> entityBlocks = SplitIntoBlocks(content, '{', '}');

        LOG_INFO("Found " + std::to_string(entityBlocks.size()) + " entities");

        // Parse each entity
        for (const auto& block : entityBlocks) {
            Entity entity = ParseEntity(block);
            map.entities.push_back(entity);
        }

        // First entity is always worldspawn
        if (!map.entities.empty()) {
            map.worldspawn = map.entities[0];
            LOG_INFO("Worldspawn has " + std::to_string(map.worldspawn.brushes.size()) + " brushes");
        }

        LOG_INFO("MAP loaded successfully");
        return map;
    }

    std::string MapLoader::RemoveComments(const std::string& content) {
        std::string result;
        result.reserve(content.size());

        bool inComment = false;
        for (size_t i = 0; i < content.size(); i++) {
            if (!inComment && i + 1 < content.size() && content[i] == '/' && content[i + 1] == '/') {
                inComment = true;
                i++; // Skip second slash
            } else if (inComment && content[i] == '\n') {
                inComment = false;
                result += '\n'; // Keep newline
            } else if (!inComment) {
                result += content[i];
            }
        }

        return result;
    }

    std::vector<std::string> MapLoader::SplitIntoBlocks(const std::string& content, char open, char close) {
        std::vector<std::string> blocks;
        int depth = 0;
        size_t blockStart = 0;

        for (size_t i = 0; i < content.size(); i++) {
            if (content[i] == open) {
                if (depth == 0) {
                    blockStart = i + 1; // Start after opening brace
                }
                depth++;
            } else if (content[i] == close) {
                depth--;
                if (depth == 0) {
                    // Extract block content
                    std::string block = content.substr(blockStart, i - blockStart);
                    block = Trim(block);
                    if (!block.empty()) {
                        blocks.push_back(block);
                    }
                }
            }
        }

        return blocks;
    }

    Entity MapLoader::ParseEntity(const std::string& block) {
        Entity entity;

        // Split block into lines
        std::istringstream stream(block);
        std::string line;

        while (std::getline(stream, line)) {
            line = Trim(line);
            if (line.empty()) continue;

            // Check if this is a brush block
            if (line[0] == '{') {
                // Find matching closing brace
                std::string brushBlock;
                brushBlock += line.substr(1); // Skip opening brace

                int braceDepth = 1;
                while (braceDepth > 0 && std::getline(stream, line)) {
                    // Count braces
                    for (char c : line) {
                        if (c == '{') braceDepth++;
                        else if (c == '}') braceDepth--;
                    }

                    if (braceDepth > 0) {
                        brushBlock += "\n" + line;
                    }
                }

                // Parse brush
                Brush brush = ParseBrush(brushBlock);
                entity.brushes.push_back(brush);
            }
            // Check if this is a property line ("key" "value")
            else if (line[0] == '"') {
                auto tokens = Tokenize(line);
                if (tokens.size() >= 2) {
                    std::string key = tokens[0];
                    std::string value = tokens[1];

                    // Remove quotes
                    if (!key.empty() && key.front() == '"' && key.back() == '"') {
                        key = key.substr(1, key.length() - 2);
                    }
                    if (!value.empty() && value.front() == '"' && value.back() == '"') {
                        value = value.substr(1, value.length() - 2);
                    }

                    if (key == "classname") {
                        entity.classname = value;
                    } else {
                        entity.properties[key] = value;
                    }
                }
            }
        }

        return entity;
    }

    Brush MapLoader::ParseBrush(const std::string& block) {
        Brush brush;

        std::istringstream stream(block);
        std::string line;

        while (std::getline(stream, line)) {
            line = Trim(line);
            if (line.empty()) continue;

            // Plane lines start with '('
            if (line[0] == '(') {
                Plane plane = ParsePlane(line);
                brush.planes.push_back(plane);
            }
        }

        return brush;
    }

    Plane MapLoader::ParsePlane(const std::string& line) {
        Plane plane;

        auto tokens = Tokenize(line);

        if (tokens.size() < 15) {
            LOG_WARNING("Invalid plane format: " + line);
            return plane;
        }

        // Parse three points
        // Format: ( x y z ) ( x y z ) ( x y z ) TEXTURE offsetX offsetY rotation scaleX scaleY

        int idx = 0;

        // Skip opening paren and parse first point
        idx++; // Skip '('
        plane.p1.x = std::stof(tokens[idx++]);
        plane.p1.y = std::stof(tokens[idx++]);
        plane.p1.z = std::stof(tokens[idx++]);
        idx++; // Skip ')'

        // Second point
        idx++; // Skip '('
        plane.p2.x = std::stof(tokens[idx++]);
        plane.p2.y = std::stof(tokens[idx++]);
        plane.p2.z = std::stof(tokens[idx++]);
        idx++; // Skip ')'

        // Third point
        idx++; // Skip '('
        plane.p3.x = std::stof(tokens[idx++]);
        plane.p3.y = std::stof(tokens[idx++]);
        plane.p3.z = std::stof(tokens[idx++]);
        idx++; // Skip ')'

        // Texture name
        plane.texture = tokens[idx++];

        // Texture alignment
        plane.offsetX = std::stof(tokens[idx++]);
        plane.offsetY = std::stof(tokens[idx++]);
        plane.rotation = std::stof(tokens[idx++]);
        plane.scaleX = std::stof(tokens[idx++]);
        plane.scaleY = std::stof(tokens[idx++]);

        // Compute plane equation
        ComputePlaneEquation(plane);

        return plane;
    }

    void MapLoader::ComputePlaneEquation(Plane& plane) {
        glm::vec3 v1 = plane.p2 - plane.p1;
        glm::vec3 v2 = plane.p3 - plane.p1;
        // Quake maps use clockwise winding, so cross product points inward.
        // We want outward normals, so we negate the result (or swap v1/v2).
        plane.normal = glm::normalize(glm::cross(v2, v1)); 
        plane.distance = glm::dot(plane.normal, plane.p1);
    }

    std::vector<std::string> MapLoader::Tokenize(const std::string& line) {
        std::vector<std::string> tokens;
        std::string current;
        bool inQuotes = false;

        for (char c : line) {
            if (c == '"') {
                if (inQuotes) {
                    // End quote - save token with quotes
                    current += c;
                    tokens.push_back(current);
                    current.clear();
                    inQuotes = false;
                } else {
                    // Start quote
                    if (!current.empty()) {
                        tokens.push_back(current);
                        current.clear();
                    }
                    current += c;
                    inQuotes = true;
                }
            } else if (IsWhitespace(c) && !inQuotes) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
            } else {
                current += c;
            }
        }

        if (!current.empty()) {
            tokens.push_back(current);
        }

        return tokens;
    }

    bool MapLoader::IsWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }

    std::string MapLoader::Trim(const std::string& str) {
        size_t start = 0;
        size_t end = str.length();

        while (start < end && IsWhitespace(str[start])) {
            start++;
        }

        while (end > start && IsWhitespace(str[end - 1])) {
            end--;
        }

        return str.substr(start, end - start);
    }

} // namespace VibeReaper
