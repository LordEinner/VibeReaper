# VibeReaper - Gothic Horror 3D Action Game

A third-person action game inspired by Bloodborne and Hellsing, featuring transforming scythe combat, rally mechanics, and gothic horror atmosphere.

## Project Overview

**Genre**: 3D Third-Person Action  
**Theme**: Gothic Horror (demons, possessed churches, cemeteries, occult)  
**Tech Stack**: C++ | SDL2 | OpenGL 3.3+ | GLM  
**Level Editor**: TrenchBroom (Quake MAP format)

## Core Features

### ⚔️ Dual Weapon System
- **Long Scythe Mode**: Two-handed scythe with wide sweeping attacks
- **Short Scythe + Shotgun Mode**: Hybrid melee/ranged combat

### 🩸 Rally Mechanic (Bloodborne-inspired)
- Recover 75% of recently lost health by attacking enemies
- 3.5-second window to rally back damage
- Encourages aggressive, risk-reward gameplay

### 🏰 Level Design
- TrenchBroom integration for professional level editing
- Quake MAP format parser
- Gothic architecture: churches, cemeteries, castles

### 🎮 Combat
- Third-person camera with lock-on
- Melee (scythe) and ranged (shotgun) combat
- Enemy AI with chase, attack, and ragdoll physics (planned)
- Satisfying combat feedback

## Technology Stack

| Component | Technology | Purpose |
|-----------|------------|---------|
| Language | C++17 | Core engine and game logic |
| Graphics | OpenGL 3.3+ | 3D rendering |
| Window/Input | SDL2 | Cross-platform window, input, audio |
| Math | GLM | Vector/matrix operations |
| Textures | stb_image | Image loading |
| Audio | SDL_mixer / stb_vorbis | Sound effects and music |
| Models | TinyOBJ / Assimp | 3D model loading |
| Physics (Future) | JoltPhysics / Bullet | Cloth sim, ragdoll |
| Level Editor | TrenchBroom | Map creation |

## Project Structure

```
VibeReaper/
├── src/
│   ├── main.cpp                    # Entry point
│   ├── Engine/                     # Engine systems
│   │   ├── Renderer.h/cpp         # OpenGL rendering
│   │   ├── Shader.h/cpp           # Shader management
│   │   ├── Mesh.h/cpp             # 3D mesh handling
│   │   ├── Texture.h/cpp          # Texture loading
│   │   ├── Camera.h/cpp           # TPP camera system
│   │   ├── Input.h/cpp            # Keyboard/gamepad input
│   │   ├── MapLoader.h/cpp        # TrenchBroom MAP parser
│   │   ├── AudioManager.h/cpp     # Sound system
│   │   └── UI.h/cpp               # User interface
│   ├── Game/                       # Game logic
│   │   ├── Player.h/cpp           # Player controller
│   │   ├── Enemy.h/cpp            # Enemy AI
│   │   ├── World.h/cpp            # Level/world management
│   │   ├── Combat.h/cpp           # Combat system
│   │   ├── Weapon.h/cpp           # Weapon base class
│   │   ├── ScytheWeapon.h/cpp     # Scythe implementation
│   │   └── Shotgun.h/cpp          # Shotgun implementation
│   └── Utils/
│       ├── Math.h                 # Math utilities
│       └── Logger.h/cpp           # Debug logging
├── assets/
│   ├── models/                    # 3D models (.obj, .fbx)
│   │   ├── player/               # Player and weapons
│   │   ├── enemies/              # Enemy models
│   │   └── environment/          # Level props
│   ├── textures/                 # Texture files
│   ├── maps/                     # TrenchBroom .map files
│   ├── shaders/                  # GLSL shaders
│   │   ├── basic.vert/frag      # Basic rendering
│   │   └── lighting.vert/frag   # Lit rendering
│   ├── audio/
│   │   ├── sfx/                 # Sound effects
│   │   └── music/               # Background music
│   └── ui/                      # UI textures/sprites
├── lib/                          # Third-party libraries
│   ├── SDL2/
│   ├── glad/
│   ├── glm/
│   └── stb/
├── CMakeLists.txt               # Build configuration
└── README.md                    # This file
```

## Development Phases

### ✅ Phase 0: Planning & Research (COMPLETED)
- [x] Technology stack selection
- [x] Asset source research
- [x] Implementation planning
- [x] TrenchBroom integration research

### ✅ Phase 1: Engine Foundation (COMPLETED)
- [x] CMake build system setup
- [x] Main game loop
- [x] SDL2 initialization
- [x] OpenGL context creation
- [x] Shader system
- [x] Basic renderer
- [x] Logger utility

### ✅ Phase 2: Basic 3D Rendering (COMPLETED)
- [x] Mesh class (VAO/VBO/EBO)
- [x] Procedural geometry generation (cube, sphere, plane)
- [x] Texture loading with stb_image
- [x] Third-person orbital camera
- [x] Phong lighting shaders (Blinn-Phong)
- [x] Render textured 3D primitives
- [x] Mouse orbit and zoom controls

### 🔄 Phase 3: TrenchBroom Integration (IN PLANNING)
- [ ] Quake MAP format parser
- [ ] Brush-to-mesh conversion (CSG to geometry)
- [ ] Entity parsing and spawning
- [ ] World/level manager
- [x] Custom FGD file for TrenchBroom
- [x] Planning and architecture design
- [ ] Test level loading

### 📋 Phase 4: Player Controller & Input
- [ ] Input manager (keyboard/mouse/gamepad)
- [ ] Player movement (WASD, analog stick)
- [ ] Camera controls
- [ ] Collision detection (AABB)
- [ ] Gravity and jumping

### 📋 Phase 5: Combat System - Dual Weapons
- [ ] Weapon base class
- [ ] Scythe weapon (long mode)
- [ ] Scythe weapon (short mode)
- [ ] Shotgun weapon
- [ ] Mode switching system
- [ ] Hitbox detection
- [ ] Attack animations

### 📋 Phase 6: Rally Mechanic
- [ ] Damage tracking system
- [ ] Rally health pool
- [ ] Rally timer (3.5s window)
- [ ] Health recovery on hit
- [ ] UI indicators (orange bar)
- [ ] Balance testing

### 📋 Phase 7: Enemy AI & Combat
- [ ] Enemy base class
- [ ] AI state machine (idle, chase, attack)
- [ ] Pathfinding (simple steering)
- [ ] Enemy types (demon, cultist, etc.)
- [ ] Enemy health and damage
- [ ] Death system

### 📋 Phase 8: Audio System
- [ ] SDL_mixer / stb_vorbis integration
- [ ] Sound effect playback
- [ ] Background music
- [ ] 3D audio positioning (optional)
- [ ] Volume controls

### 📋 Phase 9: UI & HUD
- [ ] Health bar with rally indicator
- [ ] Ammo counter
- [ ] Weapon mode indicator
- [ ] Crosshair
- [ ] Damage numbers
- [ ] Main menu
- [ ] Pause menu

### 📋 Phase 10: Advanced Physics (Future)
- [ ] JoltPhysics / Bullet integration
- [ ] Cloth simulation (player cape/coat)
- [ ] Ragdoll physics (enemy deaths)
- [ ] Advanced collision

## Build Instructions

### Prerequisites
- C++17 compiler (MSVC 2019+, GCC 9+, Clang 10+)
- CMake 3.15+
- SDL2 development libraries
- OpenGL 3.3+ support

### Windows (Visual Studio)
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Linux (GCC/Clang)
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Running
```bash
./VibeReaper  # Linux
.\Release\VibeReaper.exe  # Windows
```

## Controls

### Keyboard & Mouse
- **WASD**: Movement
- **Mouse**: Camera rotation
- **Left Click**: Scythe attack
- **Right Click**: Shotgun fire (short mode only)
- **Q**: Switch weapon mode
- **Space**: Jump
- **Shift**: Dodge/roll (planned)
- **ESC**: Pause menu

### Gamepad (Xbox Layout)
- **Left Stick**: Movement
- **Right Stick**: Camera rotation
- **RT**: Scythe attack
- **LT**: Shotgun fire (short mode only)
- **Y**: Switch weapon mode
- **A**: Jump
- **B**: Dodge/roll (planned)
- **Start**: Pause menu

## Asset Sources

### 3D Models (CC0/Free)
- **Quaternius**: CC0 low-poly models, animations
- **OpenGameArt.org**: Gothic/horror themed assets
- **itch.io**: Gothic cemetery packs, demon models
- **Sketchfab**: CC0 gothic architecture

### Textures
- **Poly Haven**: CC0 stone, wood, metal textures
- Custom dark gothic palette

### Audio
- **OpenGameArt.org**: Retro horror SFX
- **Freesound.org**: Ambient sounds (bells, wind)
- **PlayOnLoop**: Gothic/dark ambient music

## Art Direction

### Visual Style
- Low-poly aesthetic (Doom 1993 inspired)
- Gothic horror theme (Bloodborne, Castlevania)
- Dark color palette (blacks, deep reds, purples)
- Dramatic lighting (torches, moonlight)

### Environment Themes
- Possessed churches (overturned pews, demonic symbols)
- Graveyards (fog, crooked tombstones, crypts)
- Gothic castles (gargoyles, stained glass)
- Catacombs (bone-lined walls, eerie glow)

## Contributing

This is primarily an AI-assisted solo dev project, but feedback and suggestions are welcome!

## License

TBD - Will be determined once initial development is complete

## Credits

**Development**: AI-assisted development using Antigravity  
**Inspiration**: Bloodborne, Hellsing, Castlevania, classic Doom  
**Tools**: TrenchBroom, SDL2, OpenGL, GLM

---

**Current Status**: Phase 3 planning complete - TrenchBroom integration architecture designed, ready for implementation
**Last Updated**: 2025-11-27
