#include "World.h"
#include "../Utils/Logger.h"
#include <utility>

namespace VibeReaper {

    World::World() {
    }

    World::~World() {
        Unload();
    }

    bool World::LoadMap(const std::string& mapPath) {
        LOG_INFO("World: Loading map: " + mapPath);

        // Unload previous map
        Unload();

        // Parse MAP file
        map = MapLoader::LoadFromFile(mapPath);
        if (map.entities.empty()) {
            LOG_ERROR("Failed to parse map or map is empty");
            return false;
        }

        // Get worldspawn (entity 0)
        worldspawn = map.entities[0];

        if (worldspawn.classname != "worldspawn") {
            LOG_WARNING("First entity is not worldspawn, classname: " + worldspawn.classname);
        }

        // Convert worldspawn brushes to meshes
        LOG_INFO("Converting " + std::to_string(worldspawn.brushes.size()) + " brushes to meshes");
        
        for (const auto& brush : worldspawn.brushes) {
            Mesh mesh = BrushConverter::ConvertBrushToMesh(brush);
            
            // Skip empty meshes
            if (mesh.vertices.empty()) continue;

            // Setup mesh buffers
            mesh.SetupMesh();

            // Determine texture
            std::string textureName = "test_texture"; // Default fallback
            if (!brush.planes.empty()) {
                textureName = brush.planes[0].texture;
            }

            // Load texture if not in cache
            if (textureCache.find(textureName) == textureCache.end()) {
                Texture texture;
                std::string texturePath = "assets/textures/" + textureName + ".png";
                
                // Try to load texture
                if (!texture.LoadFromFile(texturePath)) {
                    LOG_WARNING("Failed to load texture: " + texturePath + ", using fallback");
                    // Try to create fallback white texture if not already created
                    // Note: We might want a shared fallback texture, but for now let's just create one per missing texture
                    // or better, map all missing textures to a special "fallback" key?
                    // Let's just create a white texture for this entry so we don't try to load it again
                    texture.CreateWhiteTexture();
                }
                
                textureCache[textureName] = std::move(texture);
            }

            // Store render object
            RenderObject obj;
            obj.mesh = std::move(mesh);
            obj.texture = &textureCache[textureName];
            levelGeometry.push_back(std::move(obj));
        }
        
        LOG_INFO("Generated " + std::to_string(levelGeometry.size()) + " render objects");

        // Spawn entities (lights, enemies, etc.)
        SpawnEntities();

        LOG_INFO("Map loaded successfully");
        return true;
    }

    void World::Unload() {
        levelGeometry.clear();
        textureCache.clear();
        map.entities.clear();
    }

    void World::Render(Shader& shader) {
        // Set model matrix to identity (level geometry is in world space)
        glm::mat4 model = glm::mat4(1.0f);
        shader.SetMat4("model", model);

        // Render all level geometry
        for (auto& obj : levelGeometry) {
            // Bind texture
            if (obj.texture) {
                obj.texture->Bind(0);
            }
            
            // Draw mesh
            obj.mesh.Draw(shader);
        }
    }

    void World::Update(float deltaTime) {
        // Future: update dynamic entities, doors, etc.
    }

    glm::vec3 World::GetPlayerSpawnPosition() const {
        // Find info_player_start entity
        for (const auto& entity : map.entities) {
            if (entity.classname == "info_player_start") {
                return entity.GetOrigin();
            }
        }

        // Default spawn if not found
        LOG_WARNING("No info_player_start found, using default spawn");
        return glm::vec3(0.0f, 0.0f, 32.0f);
    }

    float World::GetPlayerSpawnAngle() const {
        // Find info_player_start entity
        for (const auto& entity : map.entities) {
            if (entity.classname == "info_player_start") {
                return entity.GetFloat("angle", 0.0f);
            }
        }

        return 0.0f;
    }

    std::vector<const Entity*> World::GetEntitiesByClass(const std::string& classname) const {
        std::vector<const Entity*> result;
        for (const auto& entity : map.entities) {
            if (entity.classname == classname) {
                result.push_back(&entity);
            }
        }
        return result;
    }

    void World::SpawnEntities() {
        // Log entities for debugging
        for (const auto& entity : map.entities) {
            if (entity.classname == "worldspawn") {
                continue; // Already processed
            }

            LOG_INFO("Entity: " + entity.classname + " at " + 
                     std::to_string(entity.GetOrigin().x) + ", " +
                     std::to_string(entity.GetOrigin().y) + ", " +
                     std::to_string(entity.GetOrigin().z));

            // Future phases will spawn actual game objects here
            // For now, just log them
        }
    }

} // namespace VibeReaper
