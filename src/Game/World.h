#pragma once

#include "../Engine/MapLoader.h"
#include "../Engine/BrushConverter.h"
#include "../Engine/Mesh.h"
#include "../Engine/Texture.h"
#include <vector>
#include <string>
#include <map>

namespace VibeReaper {

    struct RenderObject {
        Mesh mesh;
        Texture* texture;
    };

    // World manager for level geometry and entities
    class World {
    public:
        World();
        ~World();

        // Map loading
        bool LoadMap(const std::string& mapPath);
        void Unload();

        // Rendering
        void Render(Shader& shader);
        void Update(float deltaTime);

        // Entity queries
        glm::vec3 GetPlayerSpawnPosition() const;
        float GetPlayerSpawnAngle() const;
        std::vector<const Entity*> GetEntitiesByClass(const std::string& classname) const;
        const Entity* GetWorldspawn() const { return &worldspawn; }

    private:
        // Level data
        std::vector<RenderObject> levelGeometry;
        std::map<std::string, Texture> textureCache;
        Map map;
        Entity worldspawn;

        // Spawning (stubs for now, will implement in later phases)
        void SpawnEntities();
    };

} // namespace VibeReaper
