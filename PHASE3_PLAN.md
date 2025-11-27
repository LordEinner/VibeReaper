# Phase 3: TrenchBroom Integration - Implementation Plan

**Estimated Duration**: 5-6 days
**Complexity**: High
**Dependencies**: Phase 2 complete (Mesh, Texture, Camera systems)

---

## Overview

Phase 3 adds professional level editing capabilities to VibeReaper through TrenchBroom integration. This phase is critical for rapid level iteration and enables content creation by non-programmers.

---

## Architecture Design

### Component Hierarchy

```
World (Game/World.h)
├── MapLoader (Engine/MapLoader.h) - Parses .map files
│   └── Map structure (entities, brushes, planes)
├── BrushConverter (Engine/BrushConverter.h) - Converts brushes to meshes
│   └── Geometry algorithms (plane intersection, triangulation)
└── EntitySpawner (Game/EntitySpawner.h) - Creates game objects from entities
    └── Spawn functions (lights, enemies, items)
```

### Data Flow

```
.map file (text)
    ↓
MapLoader::LoadFromFile()
    ↓
Map structure (entities, brushes)
    ↓
BrushConverter::ConvertBrushesToMeshes()
    ↓
vector<Mesh> (renderable geometry)
    ↓
World::Render()
```

---

## Implementation Steps

### Step 1: MapLoader Foundation (Day 1)

#### 1.1: Create Data Structures

**File**: `src/Engine/MapLoader.h`

```cpp
namespace VibeReaper {

struct Plane {
    glm::vec3 p1, p2, p3;           // Three points defining plane
    glm::vec3 normal;                // Computed normal
    float distance;                  // Distance from origin
    std::string texture;             // Texture name
    float offsetX, offsetY;          // Texture offset
    float rotation;                  // Texture rotation (degrees)
    float scaleX, scaleY;           // Texture scale
};

struct Brush {
    std::vector<Plane> planes;
};

struct Entity {
    std::string classname;
    std::map<std::string, std::string> properties;
    std::vector<Brush> brushes;     // Empty for point entities

    // Helper methods
    glm::vec3 GetOrigin() const;
    int GetInt(const std::string& key, int defaultValue = 0) const;
    float GetFloat(const std::string& key, float defaultValue = 0.0f) const;
    std::string GetString(const std::string& key, const std::string& defaultValue = "") const;
    glm::vec3 GetVector3(const std::string& key, const glm::vec3& defaultValue = glm::vec3(0)) const;
};

struct Map {
    std::vector<Entity> entities;
    Entity worldspawn;              // First entity (index 0)

    Entity* FindEntityByClass(const std::string& classname);
    std::vector<Entity*> FindEntitiesByClass(const std::string& classname);
};

class MapLoader {
public:
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
};

} // namespace VibeReaper
```

#### 1.2: Implement File Reading

```cpp
// MapLoader.cpp
Map MapLoader::LoadFromFile(const std::string& path) {
    // Read entire file
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open map file: " + path);
        return Map();
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Remove comments (// style)
    // Split into entity blocks
    // Parse each entity

    LOG_INFO("Loaded map: " + path);
    return map;
}
```

#### 1.3: Implement Tokenizer

```cpp
std::vector<std::string> MapLoader::Tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;

    for (char c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
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
```

**Test**: Parse simple .map with one entity, print to console

---

### Step 2: Plane and Brush Parsing (Day 1-2)

#### 2.1: Parse Plane Definition

```cpp
Plane MapLoader::ParsePlane(const std::string& line) {
    // Input: ( -64 -64 -16 ) ( -64 -63 -16 ) ( -64 -64 -15 ) STONE 0 0 0 1 1

    Plane plane;
    auto tokens = Tokenize(line);

    // Parse three points (skip parentheses)
    int idx = 0;
    plane.p1 = glm::vec3(
        std::stof(tokens[idx + 1]),
        std::stof(tokens[idx + 2]),
        std::stof(tokens[idx + 3])
    );
    idx += 5; // Skip closing paren and opening paren

    plane.p2 = glm::vec3(
        std::stof(tokens[idx + 1]),
        std::stof(tokens[idx + 2]),
        std::stof(tokens[idx + 3])
    );
    idx += 5;

    plane.p3 = glm::vec3(
        std::stof(tokens[idx + 1]),
        std::stof(tokens[idx + 2]),
        std::stof(tokens[idx + 3])
    );
    idx += 4;

    // Parse texture and alignment
    plane.texture = tokens[idx++];
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
    plane.normal = glm::normalize(glm::cross(v1, v2));
    plane.distance = glm::dot(plane.normal, plane.p1);
}
```

**Test**: Parse brush with 6 planes, verify normals point outward

---

### Step 3: BrushConverter Implementation (Day 2-3)

#### 3.1: Create BrushConverter Class

**File**: `src/Engine/BrushConverter.h`

```cpp
namespace VibeReaper {

class BrushConverter {
public:
    static Mesh ConvertBrushToMesh(const Brush& brush);
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
    static void SortWindingOrder(std::vector<glm::vec3>& vertices, const glm::vec3& normal);
    static glm::vec2 CalculateUV(const glm::vec3& vertex, const Plane& plane);
    static std::vector<unsigned int> TriangulateFace(int startIndex, int vertexCount);
};

} // namespace VibeReaper
```

#### 3.2: Implement Plane Intersection

```cpp
glm::vec3 BrushConverter::IntersectThreePlanes(const Plane& p1, const Plane& p2, const Plane& p3) {
    // Solve system of equations:
    // dot(n1, p) = d1
    // dot(n2, p) = d2
    // dot(n3, p) = d3

    glm::mat3 M(p1.normal, p2.normal, p3.normal);
    M = glm::transpose(M); // GLM stores column-major

    float det = glm::determinant(M);
    if (std::abs(det) < 0.0001f) {
        return glm::vec3(0); // Planes are parallel
    }

    glm::vec3 d(p1.distance, p2.distance, p3.distance);
    return glm::inverse(M) * d;
}
```

#### 3.3: Calculate Brush Vertices

```cpp
std::vector<glm::vec3> BrushConverter::CalculateVertices(const std::vector<Plane>& planes) {
    std::vector<glm::vec3> vertices;
    int n = planes.size();

    // Test all combinations of 3 planes
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            for (int k = j + 1; k < n; k++) {
                glm::vec3 vertex = IntersectThreePlanes(planes[i], planes[j], planes[k]);

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

    LOG_INFO("Brush has " + std::to_string(vertices.size()) + " vertices");
    return vertices;
}

bool BrushConverter::IsPointInsideBrush(const glm::vec3& point, const std::vector<Plane>& planes, float epsilon) {
    for (const auto& plane : planes) {
        float dist = glm::dot(plane.normal, point) - plane.distance;
        if (dist > epsilon) {
            return false;
        }
    }
    return true;
}
```

#### 3.4: Build Face Polygons

```cpp
std::vector<Vertex> BrushConverter::BuildFace(const Plane& plane, const std::vector<glm::vec3>& vertices) {
    std::vector<glm::vec3> faceVertices;

    // Find all vertices that lie on this plane
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
```

**Test**: Convert simple cube brush, verify 8 vertices and 12 triangles

---

### Step 4: World Management (Day 3-4)

#### 4.1: Create World Class

**File**: `src/Game/World.h`

```cpp
namespace VibeReaper {

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
    std::vector<Mesh> levelGeometry;
    std::vector<Texture> levelTextures;
    Map map;
    Entity worldspawn;

    // Spawning
    void SpawnEntities();
    void SpawnLight(const Entity& entity);
    void SpawnEnemy(const Entity& entity);
    void SpawnItem(const Entity& entity);

    // Texture management
    Texture* LoadTexture(const std::string& textureName);
    std::map<std::string, int> textureCache;
};

} // namespace VibeReaper
```

#### 4.2: Implement Map Loading

```cpp
bool World::LoadMap(const std::string& mapPath) {
    LOG_INFO("Loading map: " + mapPath);

    // Unload previous map
    Unload();

    // Parse MAP file
    map = MapLoader::LoadFromFile(mapPath);
    if (map.entities.empty()) {
        LOG_ERROR("Failed to parse map");
        return false;
    }

    // Get worldspawn (entity 0)
    worldspawn = map.entities[0];

    // Convert worldspawn brushes to meshes
    levelGeometry = BrushConverter::ConvertBrushesToMeshes(worldspawn.brushes);
    LOG_INFO("Generated " + std::to_string(levelGeometry.size()) + " meshes");

    // Spawn entities
    SpawnEntities();

    return true;
}
```

#### 4.3: Entity Spawning

```cpp
void World::SpawnEntities() {
    for (const auto& entity : map.entities) {
        if (entity.classname == "worldspawn") {
            continue; // Already processed
        }

        if (entity.classname == "info_player_start") {
            // Handled by query
        } else if (entity.classname == "light") {
            SpawnLight(entity);
        } else if (entity.classname.find("enemy_") == 0) {
            SpawnEnemy(entity);
        } else if (entity.classname.find("item_") == 0) {
            SpawnItem(entity);
        } else {
            LOG_WARNING("Unknown entity: " + entity.classname);
        }
    }
}
```

**Test**: Load simple test map, verify geometry renders

---

### Step 5: TrenchBroom Setup (Day 4)

#### 5.1: Installation

1. Download TrenchBroom from https://trenchbroom.github.io/
2. Install for your platform

#### 5.2: Game Configuration

Create `games/VibeReaper.cfg` in TrenchBroom folder:

```json
{
  "version": 1,
  "name": "VibeReaper",
  "icon": "Icon.png",
  "fileformats": [
    { "format": "Standard" }
  ],
  "filesystem": {
    "searchpath": ".",
    "packageformat": { "extension": "pak", "format": "idpak" }
  },
  "textures": {
    "package": { "type": "directory", "root": "textures" },
    "format": { "extension": "png", "format": "image" },
    "palette": "gfx/palette.lmp",
    "attribute": "wad"
  },
  "entities": {
    "definitions": [ "VibeReaper.fgd" ],
    "defaultcolor": "0.6 0.6 0.6 1.0"
  },
  "tags": {
    "brush": [],
    "brushface": []
  }
}
```

#### 5.3: Create First Test Map

1. Open TrenchBroom
2. Select VibeReaper
3. Create:
   - Simple room (4 walls, floor, ceiling)
   - Place `info_player_start`
   - Place 2-3 `light` entities
   - Apply textures (or use placeholder)
4. Save as `assets/maps/test_room.map`

**Test**: Load in VibeReaper, verify room renders

---

### Step 6: Integration and Testing (Day 5)

#### 6.1: Update main.cpp

```cpp
#include "Game/World.h"

// After Phase 2 initialization
World world;
if (!world.LoadMap("assets/maps/test_room.map")) {
    LOG_ERROR("Failed to load test map");
    return -1;
}

// Set camera to player spawn
glm::vec3 spawn = world.GetPlayerSpawnPosition();
camera.SetTarget(spawn);
camera.SetDistance(5.0f);

// In render loop
world.Render(shader);
```

#### 6.2: Testing Checklist

- [ ] Parse simple .map file successfully
- [ ] Convert brush to mesh (cube, rectangular prism)
- [ ] Render level geometry textured
- [ ] Player spawn position correct
- [ ] Lights parsed (not yet rendered)
- [ ] Multiple rooms with doorways
- [ ] Complex brush shapes (angled walls)

#### 6.3: Create Test Maps

**test_room.map**: Single room
**test_corridor.map**: Hallway with turns
**test_complex.map**: Multiple rooms, various brush shapes

---

### Step 7: Documentation and Polish (Day 5-6)

#### 7.1: Update Documentation

- README.md: Mark Phase 3 complete
- PLAN.md: Check off Phase 3 items
- Create PHASE3_SUMMARY.md

#### 7.2: Code Review

- Check for memory leaks
- Verify error handling
- Add logging for debugging
- Test edge cases (invalid .map, missing textures)

---

## Acceptance Criteria

### Must Have
- [x] Parse Quake MAP format
- [x] Convert brushes to renderable meshes
- [x] Load and render simple test level
- [x] Spawn player at info_player_start
- [x] Parse light entities (rendering in Phase 7)
- [x] FGD file created
- [x] TrenchBroom configured

### Nice to Have
- [ ] Texture loading from MAP data
- [ ] Multiple textures per level
- [ ] Entity helper utilities
- [ ] MAP validation/error reporting
- [ ] Brush face merging (coplanar optimization)

### Future Phases
- [ ] Dynamic lighting from map lights (Phase 7)
- [ ] Collision detection with level (Phase 4)
- [ ] Enemy spawning from map (Phase 7)
- [ ] BSP tree for visibility (Phase 9+)

---

## Common Pitfalls

### Issue: Reversed Faces
**Solution**: Check plane normal direction, ensure counter-clockwise winding

### Issue: Missing Geometry
**Solution**: Increase epsilon tolerance for point-inside tests

### Issue: Crash on Parse
**Solution**: Add robust error handling, validate tokenization

### Issue: TrenchBroom Config Not Found
**Solution**: Verify FGD path is correct relative to game folder

---

## Resources

- **Quake MAP Specification**: https://quakewiki.org/wiki/Quake_Map_Format
- **TrenchBroom Manual**: https://trenchbroom.github.io/manual/
- **Example MAP Files**: Quake SDK, test maps

---

## Timeline

**Day 1**: MapLoader structure, parsing foundation
**Day 2**: Plane intersection, vertex calculation
**Day 3**: Face building, mesh generation
**Day 4**: World class, entity spawning, TrenchBroom setup
**Day 5**: Integration, testing, test maps
**Day 6**: Polish, documentation, code review

---

**Status**: Ready to implement
**Next Step**: Create MapLoader.h with data structures
