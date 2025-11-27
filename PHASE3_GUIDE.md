# Phase 3: TrenchBroom Integration - Educational Guide

Welcome to Phase 3 of VibeReaper development! This guide explains how to integrate TrenchBroom, parse Quake MAP files, and render level geometry.

---

## Table of Contents

1. [Overview](#overview)
2. [What is TrenchBroom?](#what-is-trenchbroom)
3. [Quake MAP Format](#quake-map-format)
4. [Brush Geometry](#brush-geometry)
5. [Entity System](#entity-system)
6. [FGD File Format](#fgd-file-format)
7. [Implementation Walkthrough](#implementation-walkthrough)
8. [Common Issues](#common-issues)

---

## Overview

### What We're Building

In Phase 3, we'll add:
- **MAP Parser**: Read Quake .map files (text format)
- **Brush-to-Mesh Converter**: Convert CSG brushes to renderable geometry
- **Entity System**: Handle spawn points, lights, and game objects
- **World Manager**: Load and render complete levels
- **TrenchBroom Setup**: Custom FGD for VibeReaper-specific entities

### End Goal

Load a level created in TrenchBroom, walk through it, and see proper lighting from placed light entities.

---

## What is TrenchBroom?

### Overview

**TrenchBroom** is a professional-grade level editor for games using Quake-style brush-based geometry. It's:
- **Free and open-source**
- **Cross-platform** (Windows, Mac, Linux)
- **Actively maintained**
- **Used by indie developers** (DUSK, Ion Fury, Prodeus)

### Why Use TrenchBroom?

**Traditional Approach (Code-based levels):**
```cpp
// Tedious and error-prone
CreateWall(vec3(-10, 0, -10), vec3(10, 5, -10));
CreateWall(vec3(10, 0, -10), vec3(10, 5, 10));
CreateFloor(vec3(-10, 0, -10), vec3(10, 0, 10));
// ... hundreds of lines
```

**TrenchBroom Approach:**
- Visual 3D editor
- Real-time preview
- Texture alignment tools
- Copy/paste/duplicate
- Grid snapping
- Vertex manipulation

### Download

https://trenchbroom.github.io/

---

## Quake MAP Format

### File Structure

A `.map` file is **plain text** with a hierarchical structure:

```
// Entity 0 (worldspawn - level geometry)
{
"classname" "worldspawn"
// Brush 0
{
( -64 -64 -16 ) ( -64 -63 -16 ) ( -64 -64 -15 ) TEXTURE 0 0 0 1 1
( -64 -64 -16 ) ( -64 -64 -15 ) ( -63 -64 -16 ) TEXTURE 0 0 0 1 1
( -64 -64 -16 ) ( -63 -64 -16 ) ( -64 -63 -16 ) TEXTURE 0 0 0 1 1
( 64 64 16 ) ( 64 65 16 ) ( 65 64 16 ) TEXTURE 0 0 0 1 1
( 64 64 16 ) ( 65 64 16 ) ( 64 64 17 ) TEXTURE 0 0 0 1 1
( 64 64 16 ) ( 64 64 17 ) ( 64 65 16 ) TEXTURE 0 0 0 1 1
}
}
// Entity 1 (player spawn)
{
"classname" "info_player_start"
"origin" "0 0 24"
}
```

### Entity Block

```
{
"classname" "light"
"origin" "128 64 96"
"light" "200"
"_color" "1.0 0.8 0.6"
}
```

**Components:**
- **classname**: Entity type (required)
- **origin**: 3D position (x y z)
- **key-value pairs**: Entity-specific properties

### Brush Block

A brush is defined by **intersecting planes**. Each plane is defined by:

```
( x1 y1 z1 ) ( x2 y2 z2 ) ( x3 y3 z3 ) TEXTURE offsetX offsetY rotation scaleX scaleY
```

**Breaking it down:**
- **(x1 y1 z1)**: Point 1 on the plane
- **(x2 y2 z2)**: Point 2 on the plane
- **(x3 y3 z3)**: Point 3 on the plane
- **TEXTURE**: Texture name (e.g., "stone_wall")
- **offsetX, offsetY**: Texture offset in pixels
- **rotation**: Texture rotation in degrees
- **scaleX, scaleY**: Texture scale (1.0 = normal size)

**Why three points?**
Three non-collinear points uniquely define a plane in 3D space.

---

## Brush Geometry

### What is a Brush?

A **brush** is a convex 3D solid defined by intersecting planes. Think of it as carving a shape from a block of marble by cutting away with flat surfaces.

### Example: Simple Cube

A cube requires **6 planes** (one per face):

```
Top:    ( 0 0 64 ) ( 1 0 64 ) ( 0 1 64 )   // Y-up plane at Z=64
Bottom: ( 0 0 0 ) ( 0 1 0 ) ( 1 0 0 )       // Y-up plane at Z=0
Front:  ( 0 0 0 ) ( 1 0 0 ) ( 0 0 1 )       // X-right plane at Y=0
Back:   ( 0 64 0 ) ( 0 64 1 ) ( 1 64 0 )    // X-right plane at Y=64
Left:   ( 0 0 0 ) ( 0 0 1 ) ( 0 1 0 )       // Z-forward plane at X=0
Right:  ( 64 0 0 ) ( 64 1 0 ) ( 64 0 1 )    // Z-forward plane at X=64
```

### Plane Math

#### Plane Equation
A plane is defined by: **ax + by + cz = d**

Where (a, b, c) is the normal vector.

#### Plane from Three Points

Given three points P1, P2, P3:

```cpp
vec3 v1 = P2 - P1;
vec3 v2 = P3 - P1;
vec3 normal = normalize(cross(v1, v2));
float distance = dot(normal, P1);
```

**Normal**: Direction the plane "faces"
**Distance**: Distance from origin to plane

### Brush-to-Mesh Conversion

**Algorithm Overview:**

1. **Parse planes** from MAP file
2. **Calculate vertices** by finding plane intersections
3. **Determine which vertices** belong to each face
4. **Triangulate faces** (convert polygons to triangles)
5. **Calculate UVs** from texture alignment data
6. **Generate normals** per face
7. **Create Mesh** object

#### Step 1: Find Plane Intersections

Three planes intersect at a point if they're not parallel:

```cpp
vec3 IntersectThreePlanes(Plane p1, Plane p2, Plane p3) {
    mat3 M(p1.normal, p2.normal, p3.normal);
    vec3 d(p1.distance, p2.distance, p3.distance);

    if (determinant(M) == 0) {
        return vec3(0); // Planes are parallel or degenerate
    }

    return inverse(M) * d;
}
```

For a brush with N planes, test all combinations of 3 planes: **C(N, 3)** intersections.

#### Step 2: Test Vertex Validity

A vertex is valid if it's **inside or on** all planes:

```cpp
bool IsPointInsideBrush(vec3 point, vector<Plane> planes) {
    for (auto& plane : planes) {
        float dist = dot(plane.normal, point) - plane.distance;
        if (dist > EPSILON) {
            return false; // Point is outside this plane
        }
    }
    return true;
}
```

#### Step 3: Build Face Polygons

For each plane, collect all valid vertices that lie on it:

```cpp
vector<vec3> GetFaceVertices(Plane face, vector<vec3> allVertices) {
    vector<vec3> faceVerts;

    for (auto& vertex : allVertices) {
        float dist = abs(dot(face.normal, vertex) - face.distance);
        if (dist < EPSILON) {
            faceVerts.push_back(vertex);
        }
    }

    // Sort vertices in winding order (counter-clockwise)
    SortVerticesWindingOrder(faceVerts, face.normal);

    return faceVerts;
}
```

#### Step 4: Triangulate Polygons

Convert N-sided polygon to triangles (fan triangulation):

```cpp
vector<Triangle> TriangulateFace(vector<vec3> vertices) {
    vector<Triangle> triangles;

    // Simple fan triangulation from first vertex
    for (size_t i = 1; i < vertices.size() - 1; i++) {
        triangles.push_back({
            vertices[0],
            vertices[i],
            vertices[i + 1]
        });
    }

    return triangles;
}
```

#### Step 5: Calculate Texture Coordinates

MAP format stores texture alignment:

```
( ... ) ( ... ) ( ... ) TEXTURE offsetX offsetY rotation scaleX scaleY
```

**Calculate UV:**

```cpp
vec2 CalculateUV(vec3 vertex, Plane plane, TextureInfo texInfo) {
    // Project vertex onto plane's 2D texture space
    vec3 uAxis, vAxis;
    GetTextureAxes(plane.normal, uAxis, vAxis);

    float u = dot(vertex, uAxis) / texInfo.scaleX + texInfo.offsetX;
    float v = dot(vertex, vAxis) / texInfo.scaleY + texInfo.offsetY;

    // Apply rotation if needed
    if (texInfo.rotation != 0) {
        // Rotate around texture center
        // ... rotation math ...
    }

    return vec2(u, v);
}
```

---

## Entity System

### Entity Types

**Worldspawn (Entity 0)**
- Contains all level geometry (brushes)
- Global properties (skybox, ambient light)

**Point Entities**
- Position only (no brushes)
- Examples: lights, spawn points, items

**Brush Entities**
- Have both position and brushes
- Examples: doors, lifts, triggers

### Common Entities

#### info_player_start
Player spawn point.

```
{
"classname" "info_player_start"
"origin" "0 0 32"
"angle" "90"
}
```

**Properties:**
- `origin`: Spawn position
- `angle`: Facing direction (degrees, 0 = east)

#### light
Point light source.

```
{
"classname" "light"
"origin" "128 256 128"
"light" "300"
"_color" "1.0 0.9 0.8"
}
```

**Properties:**
- `origin`: Light position
- `light`: Intensity (0-1000+)
- `_color`: RGB color (0.0-1.0 range)

#### enemy_demon
Custom enemy spawn.

```
{
"classname" "enemy_demon"
"origin" "256 512 32"
"health" "100"
}
```

---

## FGD File Format

### What is FGD?

**Forge Game Data** - Defines entities for TrenchBroom.

### Basic Structure

```fgd
// Base class for all entities
@BaseClass = BaseEntity [
    targetname(string) : "Name"
]

// Point entity (no brushes)
@PointClass base(BaseEntity) color(0 255 0) size(-8 -8 -8, 8 8 8) = info_player_start : "Player Start" [
    angle(integer) : "Facing Angle" : 0
]

// Brush entity (has brushes)
@SolidClass base(BaseEntity) = func_door : "Door" [
    speed(integer) : "Speed" : 100
    angle(integer) : "Move Angle" : 0
]

// Light with preview
@PointClass base(BaseEntity) color(255 255 128) light() = light : "Light Source" [
    light(integer) : "Brightness" : 200
    _color(color255) : "Color" : "255 255 255"
]
```

### FGD Syntax

**@PointClass**: Entity without brushes
**@SolidClass**: Entity with brushes
**@BaseClass**: Reusable base (inheritance)

**Attributes:**
- `color(r g b)`: Entity color in editor
- `size(min, max)`: Bounding box for point entities
- `light()`: Shows light radius preview
- `iconsprite(path)`: Custom icon

**Properties:**
```
propertyName(type) : "Display Name" : defaultValue
```

**Types:**
- `string`: Text
- `integer`: Whole number
- `float`: Decimal number
- `color255`: RGB (0-255)
- `choices`: Dropdown menu

---

## Implementation Walkthrough

### Step 1: Create MapLoader Class

**Files:**
- `src/Engine/MapLoader.h`
- `src/Engine/MapLoader.cpp`

**Key structures:**

```cpp
struct Plane {
    glm::vec3 normal;
    float distance;
    std::string texture;
    float offsetX, offsetY;
    float rotation;
    float scaleX, scaleY;
};

struct Brush {
    std::vector<Plane> planes;
};

struct Entity {
    std::string classname;
    std::map<std::string, std::string> properties;
    std::vector<Brush> brushes; // Empty for point entities

    glm::vec3 GetOrigin() const;
    float GetFloat(const std::string& key, float defaultValue) const;
};

struct Map {
    std::vector<Entity> entities;
    Entity worldspawn; // Entity 0
};
```

**API:**

```cpp
class MapLoader {
public:
    static Map LoadFromFile(const std::string& path);

private:
    static Entity ParseEntity(const std::string& block);
    static Brush ParseBrush(const std::string& block);
    static Plane ParsePlane(const std::string& line);
    static std::vector<std::string> TokenizeLine(const std::string& line);
};
```

### Step 2: Implement MAP Parser

**Parsing Strategy:**

1. Read entire file as string
2. Split into entity blocks (balanced braces)
3. For each entity:
   - Parse key-value pairs
   - Parse brush blocks
4. For each brush:
   - Parse plane definitions

**Tokenization Example:**

```cpp
// Input: ( -64 -64 -16 ) ( -64 -63 -16 ) ( -64 -64 -15 ) STONE 0 0 0 1 1
// Output: ["(", "-64", "-64", "-16", ")", "(", "-64", "-63", "-16", ")", ...]
```

### Step 3: Create BrushConverter Class

**Files:**
- `src/Engine/BrushConverter.h`
- `src/Engine/BrushConverter.cpp`

**API:**

```cpp
class BrushConverter {
public:
    static Mesh ConvertBrushToMesh(const Brush& brush);
    static std::vector<Mesh> ConvertBrushesToMeshes(const std::vector<Brush>& brushes);

private:
    static std::vector<glm::vec3> CalculateVertices(const std::vector<Plane>& planes);
    static glm::vec3 IntersectThreePlanes(const Plane& p1, const Plane& p2, const Plane& p3);
    static bool IsPointInside(const glm::vec3& point, const std::vector<Plane>& planes);
    static std::vector<Vertex> BuildFace(const Plane& plane, const std::vector<glm::vec3>& vertices);
    static void SortWindingOrder(std::vector<glm::vec3>& vertices, const glm::vec3& normal);
};
```

### Step 4: Create World Class

**Files:**
- `src/Game/World.h`
- `src/Game/World.cpp`

**API:**

```cpp
class World {
public:
    bool LoadMap(const std::string& mapPath);
    void Render(Shader& shader);
    void Update(float deltaTime);

    // Entity queries
    glm::vec3 GetPlayerSpawnPosition() const;
    std::vector<Entity> GetEntitiesByClass(const std::string& classname) const;

private:
    std::vector<Mesh> levelGeometry;
    std::vector<Entity> entities;
    Entity worldspawn;

    void SpawnEntities();
    void SpawnLight(const Entity& lightEntity);
    void SpawnEnemy(const Entity& enemyEntity);
};
```

### Step 5: Create VibeReaper FGD

**File:** `assets/VibeReaper.fgd`

```fgd
// Base class
@BaseClass = BaseEntity [
    targetname(string) : "Name"
]

// Player spawn
@PointClass base(BaseEntity) color(0 255 0) size(-16 -16 0, 16 16 64) = info_player_start : "Player Spawn" [
    angle(integer) : "Facing Angle" : 0 : "Direction player faces (0=East, 90=North, 180=West, 270=South)"
]

// Light
@PointClass base(BaseEntity) color(255 255 128) light() = light : "Light Source" [
    light(integer) : "Brightness" : 200 : "Light intensity (0-1000)"
    _color(color255) : "Color" : "255 255 255" : "RGB color of light"
    style(choices) : "Style" : 0 : "Light animation style" = [
        0 : "Normal"
        10 : "Flickering"
        2 : "Slow pulse"
    ]
]

// Demons
@PointClass base(BaseEntity) color(255 0 0) size(-24 -24 0, 24 24 64) = enemy_demon : "Lesser Demon" [
    health(integer) : "Health" : 50
    patrol(choices) : "Patrol" : 0 = [
        0 : "Stationary"
        1 : "Patrol"
    ]
]

// Cultists
@PointClass base(BaseEntity) color(128 0 128) size(-16 -16 0, 16 16 72) = enemy_cultist : "Possessed Cultist" [
    health(integer) : "Health" : 100
    weapon(choices) : "Weapon" : 0 = [
        0 : "Melee"
        1 : "Ranged"
    ]
]

// Health pickup
@PointClass base(BaseEntity) color(0 255 0) size(-8 -8 -8, 8 8 8) model({ "path": ":models/pickup_health.obj" }) = item_health : "Health Pickup" [
    amount(integer) : "Health Amount" : 25
]

// Door
@SolidClass base(BaseEntity) = func_door : "Door" [
    speed(integer) : "Speed" : 100
    wait(integer) : "Wait Before Close" : 3
    angle(integer) : "Move Angle" : 0
]
```

### Step 6: TrenchBroom Setup

1. **Download TrenchBroom** from https://trenchbroom.github.io/
2. **Configure VibeReaper Game:**
   - Preferences → Games
   - Add new game config
   - Point to VibeReaper directory
   - Set FGD path to `assets/VibeReaper.fgd`
3. **Create First Map:**
   - New map
   - Add brushes (walls, floor, ceiling)
   - Place entities (info_player_start, lights)
   - Apply textures
   - Export: `assets/maps/test_level.map`

### Step 7: Integration in main.cpp

```cpp
#include "Game/World.h"

// In main()
World world;
if (!world.LoadMap("assets/maps/test_level.map")) {
    LOG_ERROR("Failed to load map");
    return -1;
}

// Set player spawn
glm::vec3 playerSpawn = world.GetPlayerSpawnPosition();
player.SetPosition(playerSpawn);

// In render loop
world.Render(shader);
```

---

## Common Issues & Solutions

### Issue: Reversed Face Normals
**Symptom:** Walls invisible from certain angles
**Cause:** Winding order incorrect
**Solution:** Ensure counter-clockwise vertex order when viewed from front

### Issue: Missing Faces
**Symptom:** Holes in geometry
**Cause:** Plane intersection calculation imprecise
**Solution:** Increase epsilon tolerance, check for degenerate planes

### Issue: Texture Misalignment
**Symptom:** Textures don't match TrenchBroom preview
**Cause:** UV calculation incorrect
**Solution:** Verify texture axis calculation, check rotation

### Issue: Brush Turned Inside-Out
**Symptom:** Brush is invisible or renders backwards
**Cause:** All plane normals pointing inward
**Solution:** Flip plane normals during parsing

### Issue: Crash on Complex Brushes
**Symptom:** Crash with many-sided brushes
**Cause:** Too many plane intersections
**Solution:** Limit complexity or optimize intersection testing

---

## Testing Strategy

### Test Map 1: Simple Room
- 6 brushes (floor, ceiling, 4 walls)
- 1 info_player_start
- 1 light
- **Goal:** Verify basic parsing and rendering

### Test Map 2: Multiple Rooms
- Connected rooms with doorways
- Multiple lights
- Various brush shapes
- **Goal:** Verify complex geometry

### Test Map 3: Entities
- Enemy spawns
- Item pickups
- Multiple lights
- **Goal:** Verify entity system

---

## Performance Considerations

### Optimization Strategies

1. **Merge coplanar faces** - Reduce triangle count
2. **Spatial partitioning** - Only render visible sectors
3. **Occlusion culling** - Don't render hidden geometry
4. **Texture atlasing** - Reduce draw calls
5. **Static batching** - Combine meshes that don't move

### BSP Tree (Advanced - Phase 4+)
Quake uses Binary Space Partitioning for visibility:
- Pre-calculate which areas see which
- Only render visible leafs
- ~100x performance improvement for large maps

---

## Resources

### Documentation
- **Quake MAP Format**: https://quakewiki.org/wiki/Quake_Map_Format
- **TrenchBroom Manual**: https://trenchbroom.github.io/manual/
- **FGD Specification**: https://developer.valvesoftware.com/wiki/FGD

### Example Implementations
- **Quake Engine Source**: https://github.com/id-Software/Quake
- **ericw-tools**: MAP compiler tools for reference

### TrenchBroom Resources
- **Texture Packs**: https://www.quaddicted.com/
- **Tutorial Videos**: Search "TrenchBroom tutorial" on YouTube

---

## Summary

Phase 3 introduces **professional level editing** to VibeReaper:

- **MAP Parser**: Read Quake format levels
- **Brush System**: Convert CSG to meshes
- **Entity System**: Spawn objects from map data
- **TrenchBroom**: Visual level editor
- **FGD**: Custom entity definitions

This unlocks rapid level iteration and enables non-programmers to create content!

---

**Next**: Phase 4 will add player movement and collision with level geometry.
