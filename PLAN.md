# VibeReaper Implementation Checklist

> AI-friendly checklist for tracking development progress across all phases

---

## 📁 Phase Documentation

Detailed guides and planning documents are located in [`docs/phases/`](docs/phases/):

### Phase 1: Engine Foundation
- [phase1-guide.md](docs/phases/phase1-guide.md) - Educational guide

### Phase 2: Basic 3D Rendering
- [phase2-guide.md](docs/phases/phase2-guide.md) - Educational guide
- [phase2-review.md](docs/phases/phase2-review.md) - Code review (Grade: A-)
- [phase2-summary.md](docs/phases/phase2-summary.md) - Completion summary

### Phase 3: TrenchBroom Integration
- [phase3-guide.md](docs/phases/phase3-guide.md) - Educational guide
- [phase3-plan.md](docs/phases/phase3-plan.md) - Implementation roadmap
- [phase3-kickoff.md](docs/phases/phase3-kickoff.md) - Planning summary

---

## Phase 1: Engine Foundation (3-4 days) ✅ COMPLETED

### Build System
- [x] Create CMakeLists.txt with C++17 standard
- [x] Configure SDL2 linking (find_package or bundled)
- [x] Configure OpenGL + GLAD linkage
- [x] Add GLM as header-only library
- [x] Set up Debug/Release configurations
- [x] Configure asset copying to build directory
- [x] Test build on Windows
- [ ] Test build on Linux - *Not tested yet*

### Main Loop
- [x] Create src/main.cpp with SDL2 initialization
- [x] Initialize SDL video subsystem
- [x] Initialize SDL audio subsystem
- [x] Initialize SDL game controller subsystem
- [x] Create OpenGL 3.3 core context
- [x] Initialize GLAD for OpenGL function loading
- [x] Implement main game loop (event → update → render)
- [x] Add delta time calculation
- [x] Add FPS counter
- [x] Implement clean shutdown sequence

### Engine Core
- [x] Create src/Engine/Renderer.h/cpp
  - [x] Initialize() - set up OpenGL state
  - [x] Clear() - clear buffers
  - [x] SetViewport()
  - [x] SwapBuffers()
  - [x] Global state management (wireframe, vsync)
- [x] Create src/Engine/Shader.h/cpp
  - [x] Load vertex shader from file
  - [x] Load fragment shader from file
  - [x] Compile shaders with error checking
  - [x] Link shader program
  - [x] Uniform setters (SetMat4, SetVec3, SetFloat, etc.)
  - [x] Shader caching
- [x] Create src/Utils/Logger.h/cpp
  - [x] Log levels (INFO, WARNING, ERROR, DEBUG)
  - [x] Console output
  - [x] File output
  - [x] Timestamp formatting
  - [x] Macros: LOG_INFO, LOG_WARNING, LOG_ERROR

### Verification
- [x] Window opens successfully
- [x] Clear color renders (test with blue/red screen)
- [x] FPS counter shows stable 60 FPS
- [x] No OpenGL errors
- [x] Logger outputs to console and file

---

## Phase 2: Basic 3D Rendering (3-4 days) ✅ COMPLETED

### Mesh System
- [x] Create src/Engine/Mesh.h/cpp
  - [x] Vertex struct (position, normal, texCoord)
  - [x] VAO/VBO/EBO setup
  - [x] SetupMesh() function
  - [x] Draw(Shader&) function
  - [x] Indexed drawing support
- [x] Create procedural cube mesh
- [x] Create procedural sphere mesh
- [x] Create procedural plane mesh

### Texture System
- [x] Download stb_image.h to lib/stb/
- [x] Create src/Engine/Texture.h/cpp
  - [x] Load image with stb_image
  - [x] Generate OpenGL texture
  - [x] Set texture parameters (filtering, wrapping)
  - [x] Texture cache to avoid duplicates
  - [x] Bind(int slot) function
- [x] Test with simple test texture

### Camera System
- [x] Create src/Engine/Camera.h/cpp
  - [x] Position, rotation, target
  - [x] Third-person offset (distance, height)
  - [x] Follow target smoothly (lerp)
  - [x] Mouse/gamepad orbit controls
  - [x] Calculate view matrix (lookAt)
  - [x] Calculate projection matrix (perspective)
  - [ ] Wall collision raycast (prevent clipping) - *Deferred to Phase 4*

### Shaders
- [x] Create assets/shaders/basic.vert
  - [x] Transform vertices with MVP matrices
  - [x] Pass normal and UV to fragment shader
- [x] Create assets/shaders/basic.frag
  - [x] Sample diffuse texture
  - [x] Simple diffuse lighting
  - [x] Output final color
- [x] Create assets/shaders/lighting.vert
- [x] Create assets/shaders/lighting.frag
  - [x] Phong/Blinn-Phong lighting
  - [ ] Multiple point lights support - *Deferred to Phase 3+*
  - [x] Ambient + diffuse + specular

### Verification
- [x] Render textured cube
- [x] Cube rotates smoothly
- [x] Lighting affects cube appearance
- [x] Camera orbits around cube
- [x] No Z-fighting or visual artifacts

### Testing & Documentation
- [x] Code review completed
- [x] Automated test suite created
- [x] Documentation updated

---

## Phase 3: TrenchBroom Integration (5-6 days) ✅ COMPLETED

### Planning & Design
- [x] Research Quake MAP format specification
- [x] Create PHASE3_GUIDE.md (educational documentation)
- [x] Create PHASE3_PLAN.md (implementation plan)
- [x] Design MapLoader architecture and API
- [x] Design BrushConverter architecture
- [x] Design World class structure
- [x] Create custom VibeReaper.fgd file

### MAP Format Parser
- [x] Create src/Engine/MapLoader.h/cpp
  - [x] Read .map file as text
  - [x] Parse brush blocks
  - [x] Parse entity blocks
  - [x] Extract key-value pairs from entities
- [x] Create Brush struct
  - [x] List of Plane definitions
  - [x] Texture information per plane
- [x] Create Plane struct
  - [x] Normal, distance
  - [x] Texture name
  - [x] UV offset, scale, rotation
- [x] Create Entity struct
  - [x] classname (entity type)
  - [x] properties (key-value map)
  - [x] position

### Brush-to-Mesh Conversion
- [x] Implement plane intersection algorithm
  - [x] Calculate vertices from intersecting planes
  - [x] Form convex polyhedron
- [x] Triangulate polygon faces
- [x] Apply texture coordinates from plane UV data
- [x] Generate Mesh from brush geometry
- [x] Combine multiple brushes into level geometry

### World Management
- [x] Create src/Game/World.h/cpp
  - [x] LoadMap(string path)
  - [x] Store level geometry (vector<Mesh>)
  - [x] Store entities (vector<Entity>)
  - [x] SpawnEntitiesFromMap()
  - [x] Render(Shader&)
- [x] Handle "info_player_start" entity (player spawn)
- [x] Handle "light" entity (light sources)
- [x] Handle custom entities (enemies, items)

### TrenchBroom Setup
- [x] Create custom VibeReaper.fgd file
  - [x] Define info_player_start
  - [x] Define light with color/intensity
  - [x] Define enemy_demon
  - [x] Define enemy_cultist
  - [x] Define items and powerups
  - [x] Define func entities (doors, triggers)
  - [x] Define ambient sounds
  - [x] Define props (candles, tombstones, statues)
- [x] Create test map in TrenchBroom
  - [x] Simple room with walls, floor, ceiling
  - [x] Player spawn point
  - [x] 2-3 lights
  - [x] Test textures
- [x] Export test.map and load in game

### Texture System Enhancements
- [x] Implement texture coordinate transformations
  - [x] Texture scale (multiply by scale factor)
  - [x] Texture rotation (negate angle for Quake convention)
  - [x] Texture offset application
  - [x] Coordinate normalization
- [x] Implement pixel-perfect texture filtering
  - [x] GL_NEAREST_MIPMAP_NEAREST for minification
  - [x] GL_NEAREST for magnification
  - [x] Sharp, pixelated rendering for stylized aesthetic

### Development Tools
- [x] VS Code build configuration
  - [x] CMake configure, build, clean, rebuild tasks
  - [x] Launch configuration with debugger support
  - [x] Default build task (Ctrl+Shift+B)

### Verification
- [x] Load simple .map file successfully
- [x] Level geometry renders correctly
- [x] Textures applied to brushes with correct transformations
- [x] Texture coordinates match TrenchBroom editor
- [x] Pixel-perfect sharp rendering
- [x] Player spawns at info_player_start
- [x] Light entities processed correctly

---

## Phase 4: Player Controller & Input (3-4 days) ✅ COMPLETED

**Goal**: Implement responsive player movement, camera control, and basic physics for third-person gameplay.

### 4.1 Input System Foundation

**Priority**: HIGH | **Estimated Time**: 4-6 hours

Create a unified input abstraction that supports both keyboard/mouse and gamepad input.

- [x] Create src/Engine/Input.h/cpp
  - [x] Keyboard state tracking (current + previous frame)
  - [x] `bool IsKeyPressed(SDL_Scancode)` - continuous hold detection
  - [x] `bool IsKeyJustPressed(SDL_Scancode)` - single frame press
  - [x] Mouse state tracking
  - [x] `glm::vec2 GetMouseDelta()` - relative mouse movement
  - [x] `bool IsMouseButtonPressed(int button)`
  - [x] Gamepad detection and opening (SDL_GameController)
  - [x] `float GetAxis(SDL_GameControllerAxis)` - analog stick input (-1 to 1)
  - [x] `bool IsButtonPressed(SDL_GameControllerButton)`
  - [x] `bool IsButtonJustPressed(SDL_GameControllerButton)`
  - [x] `void Update()` - call at start of each frame to update state
  - [x] `void ProcessEvent(SDL_Event&)` - integrate with main event loop

**Implementation Notes**:
- Store previous frame state for "just pressed" detection
- Normalize mouse delta by delta time for framerate independence
- Support hot-plugging gamepads (detect connects/disconnects)
- Add deadzone for analog sticks (0.15f recommended)

**Test Cases**:
- [x] Keyboard input detected correctly
- [x] Mouse delta calculates properly
- [x] Gamepad connects and reads input
- [x] "Just pressed" fires only once per press
- [x] Multiple keys can be pressed simultaneously

### 4.2 Player Movement Controller

**Priority**: HIGH | **Estimated Time**: 6-8 hours

Implement player character with smooth movement, rotation, and basic physics.

- [x] Create src/Game/Player.h/cpp
  - [x] Member variables:
    - [x] `glm::vec3 position` - world position
    - [x] `glm::vec3 velocity` - current velocity
    - [x] `float yaw` - horizontal rotation (Y-axis)
    - [x] `float pitch` - vertical look angle (for animations)
    - [x] `bool isGrounded` - on floor or airborne
    - [x] `float moveSpeed` - base movement speed (6.0f)
    - [x] `float jumpStrength` - initial jump velocity (8.0f)
    - [x] `float gravity` - downward acceleration (-20.0f)
  - [x] Core methods:
    - [x] `void ProcessInput(Input& input, Camera& camera, float deltaTime)`
    - [x] `void Update(float deltaTime)` - apply physics, update position
    - [x] `void Render(Shader& shader)` - draw player model
    - [x] `void Jump()` - apply upward velocity if grounded
    - [x] `glm::vec3 GetForward()` - direction player is facing

- [x] WASD / Left Stick Movement
  - [x] Read input from Input system
  - [x] Calculate movement direction relative to camera
  - [x] Apply acceleration/deceleration (lerp velocity)
  - [x] Normalize diagonal movement (prevent faster diagonal speed)
  - [x] Rotate player to face movement direction (smooth rotation)

- [x] Mouse / Right Stick Camera Control
  - [x] Read mouse delta or right stick
  - [x] Update camera yaw (horizontal rotation)
  - [x] Update camera pitch with limits (via Camera class)
  - [x] Player forward vector based on movement input

- [x] Gravity and Jumping
  - [x] Apply gravity acceleration when not grounded
  - [x] Terminal velocity cap (-50.0f)
  - [x] Jump applies upward velocity
  - [x] Ground check before allowing jump

**Movement Feel Tuning**:
- Movement speed: 6.0 units/sec
- Acceleration time: 0.15 seconds
- Rotation speed: 12.0 radians/sec
- Jump height: ~2-3 units (jumpStrength 8.0)
- Gravity: -20.0 units/sec²

**Test Cases**:
- [x] WASD moves player correctly
- [x] Gamepad left stick moves player
- [x] Player rotates to face movement direction
- [x] Camera follows player smoothly
- [x] Jump works and gravity applies
- [x] Movement feels responsive and smooth

### 4.3 Basic Collision Detection

**Priority**: MEDIUM | **Estimated Time**: 4-6 hours

Implement AABB collision to prevent player from passing through walls and floors.

- [x] Create collision structures
  - [x] `struct AABB` in src/Engine/Collision.h
    - [x] `glm::vec3 min, max` - bounding box corners
    - [x] `bool Intersects(const AABB& other)` - overlap test
  - [x] AABB helper functions (FromCenterAndExtents, GetCenter, GetSize)
  - [x] CollisionResult structure for detailed collision info

- [x] Collision Detection Functions
  - [x] TestAABB - basic AABB vs AABB test
  - [x] ResolveAABB - calculate minimum translation vector
  - [x] RaycastAABB - ray vs AABB intersection
  - [x] SlideVelocity - project velocity along surface

- [x] Collision Framework Ready
  - [x] Collision system implemented and ready for integration
  - [ ] Integration with World (deferred - not critical for Phase 4)
  - [ ] Player-World collision resolution (deferred)
  - [ ] Ground detection with raycast (using temporary floor at y=0 for now)

**Implementation Notes**:
- Player AABB size: 0.8m × 1.75m (51.2 × 112 MAP units at 64 units/meter)
- Dimensions defined using elegant `_u` literal: `WIDTH = 0.8_u`, `HEIGHT = 1.75_u`
- Collision system is complete and ready for use
- Full world collision integration deferred to allow gameplay testing
- Temporary ground check at y=0 for basic testing

**Test Cases**:
- [x] Collision structures created and functional
- [x] AABB intersection tests work correctly
- [ ] Player-world collision (deferred)
- [ ] Wall sliding (deferred)
- [x] Basic ground detection (temporary y=0 check)

### 4.4 Camera Integration

**Priority**: MEDIUM | **Estimated Time**: 2-4 hours

Update Camera class to follow player and handle occlusion.

- [x] Update src/Engine/Camera.h/cpp
  - [x] `void FollowTarget(glm::vec3 targetPos, float deltaTime)`
  - [x] Smooth lerp to target position (lerp factor 0.1)
  - [x] Configurable offset via SetDistance() and SetRotation()
  - [x] Existing SetRotation() handles yaw/pitch

- [x] Camera Follow Logic
  - [x] Smooth lerp toward player position
  - [x] Apply yaw/pitch from mouse/gamepad input
  - [x] Orbit camera system works with player follow

- [ ] Wall Collision Raycast (Optional - deferred)
  - [ ] Raycast from player to camera position
  - [ ] If hit wall, pull camera closer
  - [ ] Smooth zoom in/out

**Test Cases**:
- [x] Camera follows player position
- [x] Camera rotation responds to mouse/stick
- [x] Camera maintains offset distance
- [x] Smooth lerping (no jitter)

### 4.5 Integration & Polish

**Priority**: MEDIUM | **Estimated Time**: 2-3 hours

Integrate all systems into main game loop and tune for feel.

- [x] Update src/main.cpp
  - [x] Create Input instance
  - [x] Create Player instance
  - [x] Call `input.ProcessEvent()` in event loop
  - [x] Call `input.Update()` at frame start
  - [x] Call `player.ProcessInput(input, camera, deltaTime)`
  - [x] Call `player.Update(deltaTime)`
  - [x] Call `camera.FollowTarget(player.position, deltaTime)`
  - [x] Call `player.Render(shader)`

- [x] Tuning & Feel
  - [x] Movement speed: 5.0_u (320 MAP units/sec) for comfortable exploration
  - [x] Camera sensitivity: 0.15 for mouse, 3.0 for gamepad
  - [x] Camera invert settings (horizontal/vertical toggles)
  - [x] Jump removed per design requirements
  - [x] Supports both keyboard/mouse and gamepad

- [x] Camera System Enhancements
  - [x] Third-person camera orbiting player
  - [x] Camera targets player center (0.875m height)
  - [x] Camera distance: 5.0_u (320 MAP units)
  - [x] Smooth camera following with lerp

- [x] Player Improvements
  - [x] Camera-relative movement (movement matches camera orientation)
  - [x] Player model rotates to face movement direction
  - [x] Player dimensions: 0.8m × 1.75m (human-sized)
  - [x] Player positioned correctly (feet at y=0, top at 1.75m)
  - [x] Green cube visualization for player character

- [x] Scale System Implementation
  - [x] Global MAP_UNITS_PER_METER constant (64 units = 1 meter)
  - [x] User-defined literal `_u` for elegant meter-to-unit conversion
  - [x] Examples: `1.75_u`, `5.0_u`, `8.0_u`
  - [x] TrenchBroom GameConfig updated (texture scale 0.25)
  - [x] Texture UV calculation fixed to match TrenchBroom

- [ ] Debug Visualization (Optional - deferred)
  - [ ] Render player AABB (wireframe)
  - [ ] Render world AABBs
  - [ ] Render player forward vector

### Verification Checklist

- [x] Player moves smoothly with WASD
- [x] Player moves smoothly with gamepad left stick
- [x] Camera rotates with mouse movement
- [x] Camera rotates with gamepad right stick
- [x] Movement direction matches camera orientation (camera-relative)
- [x] Player model rotates to face movement direction
- [x] Player stands on ground (feet at y=0, height 1.75m)
- [x] Third-person camera orbits player at correct distance
- [x] Camera targets player center (not ground level)
- [x] Texture scale matches TrenchBroom (0.25 scale)
- [x] Texture orientation correct on all surfaces
- [x] Scale system with _u literal works elegantly
- [x] Camera invert settings (horizontal/vertical) functional
- [ ] Player cannot walk through walls (deferred - collision integration)
- [x] Player does not fall through floor (basic y=0 check implemented)
- [ ] Player slides along walls (deferred - collision integration)
- [x] Movement feels responsive and satisfying
- [x] Player renders and rotates correctly
- [x] Camera follows player smoothly
- [x] No jitter or stuttering
- [x] Build succeeds without errors

### Success Criteria

Phase 4 is complete when:
1. ✅ Player can freely explore with keyboard/mouse AND gamepad
2. ✅ Movement feels smooth and responsive
3. ⚠️ Basic physics work (full collision deferred to avoid blocking progress)
4. ✅ Camera follows player with appropriate feel
5. ✅ Core systems implemented and functional

**Note**: Full world collision integration has been intentionally deferred to allow for testing and iteration on movement feel. The collision system is complete and ready for integration in a future update.

### Next Phase Preparation

After Phase 4, we'll move to Phase 5: Combat System. Consider:
- Player model placeholder (cube or capsule for now)
- Weapon attachment points on player
- Attack input binding (left mouse / RT button)

---

## Phase 5: Combat System - Dual Weapons (5-6 days)

### Weapon Base
- [ ] Create src/Game/Weapon.h/cpp
  - [ ] Virtual Attack()
  - [ ] Virtual Update(deltaTime)
  - [ ] Virtual Render(Shader&)

### Scythe Weapon
- [ ] Create src/Game/ScytheWeapon.h/cpp
  - [ ] Long scythe model reference
  - [ ] Short scythe model reference
  - [ ] Current weapon mode (LONG or SHORT)
  - [ ] Attack cooldown timer
  - [ ] IsAttacking flag
  - [ ] PerformAttack()
  - [ ] CheckHitbox() - cone-shaped area
  - [ ] SwitchMode(WeaponMode)
- [ ] Download/create long scythe 3D model
- [ ] Download/create short scythe blade 3D model
- [ ] Implement hitbox detection (cone/arc in front of player)
- [ ] Different attack ranges for long vs short mode
- [ ] Attack animations (placeholders initially)

### Shotgun Weapon
- [ ] Create src/Game/Shotgun.h/cpp
  - [ ] Ammo count (current, max)
  - [ ] Reload timer
  - [ ] Fire rate cooldown
  - [ ] Fire(origin, direction)
  - [ ] Reload()
  - [ ] CanFire()
- [ ] Download/create shotgun 3D model
- [ ] Hitscan raycasting (multiple pellets)
- [ ] Cone spread for pellets
- [ ] Camera shake on fire
- [ ] Only active in SHORT_SCYTHE mode

### Combat Manager
- [ ] Create src/Game/Combat.h/cpp
  - [ ] Track active attacks
  - [ ] ApplyDamage(enemy, amount)
  - [ ] Visual effects (later: particles)
  - [ ] Sound effects integration

### Mode Switching
- [ ] Player.weaponMode enum (LONG_SCYTHE, SHORT_SCYTHE_SHOTGUN)
- [ ] SwitchWeaponMode() function
- [ ] Smooth transition (0.5-1s)
- [ ] Cannot attack during transition
- [ ] Weapon model swaps visually

### Verification
- [ ] Press Q / Y button to switch modes
- [ ] Long scythe: wide attack, high damage
- [ ] Short scythe: faster attacks, medium damage
- [ ] Shotgun fires only in short mode
- [ ] Hitbox detection works for both modes
- [ ] Ammo counter displays correctly

---

## Phase 6: Rally Mechanic (2-3 days)

### Rally System Implementation
- [ ] Add to Player class:
  - [ ] recentDamagePool (float)
  - [ ] rallyWindowTimer (float)
  - [ ] RALLY_WINDOW constant (3.5f)
  - [ ] RALLY_PERCENTAGE constant (0.75f)
- [ ] Modify Player::TakeDamage(amount)
  - [ ] Subtract health
  - [ ] Set recentDamagePool = amount * RALLY_PERCENTAGE
  - [ ] Reset rallyWindowTimer = RALLY_WINDOW
- [ ] Implement Player::OnSuccessfulHit(Enemy*)
  - [ ] Check if rallyWindowTimer > 0
  - [ ] Calculate rally amount (min of damage dealt and pool)
  - [ ] Add health
  - [ ] Reduce recentDamagePool
- [ ] Implement Player::UpdateRally(deltaTime)
  - [ ] Decrement rallyWindowTimer
  - [ ] If expired, clear recentDamagePool
- [ ] Call UpdateRally in Player::Update

### Visual Feedback
- [ ] Health bar split into sections:
  - [ ] Red: current health
  - [ ] Orange: rallied health (recoverable)
  - [ ] Gray: lost health
- [ ] Orange section fades over rally window
- [ ] Green damage numbers on rally recovery
- [ ] Satisfying sound effect on rally

### Balancing
- [ ] Test rally percentage (50-80% range)
- [ ] Test rally window (2-5 seconds range)
- [ ] Adjust for feel and balance

### Verification
- [ ] Take damage from enemy
- [ ] Orange rally bar appears
- [ ] Attack enemy within window
- [ ] Health recovers (green numbers)
- [ ] Rally expires if no attack
- [ ] Feels rewarding and encourages aggression

---

## Phase 7: Enemy AI & Combat (5-6 days)

### Enemy Base Class
- [ ] Create src/Game/Enemy.h/cpp
  - [ ] AIState enum (IDLE, PATROL, CHASE, ATTACK, HURT, DEAD)
  - [ ] position, rotation
  - [ ] health, maxHealth
  - [ ] detectionRange, attackRange
  - [ ] movementSpeed
  - [ ] UpdateAI(Player*, deltaTime)
  - [ ] ChasePlayer(Player*)
  - [ ] AttackPlayer(Player*)
  - [ ] TakeDamage(amount, Player* attacker)
  - [ ] Die()
  - [ ] Render(Shader&)

### AI Behavior
- [ ] IDLE state: stand still or wander
- [ ] PATROL state: follow waypoints (optional)
- [ ] CHASE state: move toward player when in range
- [ ] ATTACK state: perform attack animation
- [ ] HURT state: brief stun on damage
- [ ] DEAD state: trigger ragdoll (or fall animation)

### Enemy Types
- [ ] Lesser Demon
  - [ ] Low health (50 HP)
  - [ ] Fast movement
  - [ ] Melee attack
  - [ ] Download/create demon model
- [ ] Possessed Cultist
  - [ ] Medium health (100 HP)
  - [ ] Medium speed
  - [ ] Combo attacks
  - [ ] Download/create cultist model
- [ ] (Optional) Gargoyle
  - [ ] Flying enemy
  - [ ] Dive attack
- [ ] (Optional) Hellhound
  - [ ] Very fast
  - [ ] Low health
  - [ ] Pack behavior

### Pathfinding
- [ ] Simple steering toward player
- [ ] Obstacle avoidance (raycasts)
- [ ] Optional: grid-based pathfinding for complex levels

### Combat Integration
- [ ] Enemies call Player::TakeDamage() on hit
- [ ] Player scythe/shotgun calls Enemy::TakeDamage()
- [ ] Enemy triggers Player::OnSuccessfulHit() for rally
- [ ] Enemy spawning from map entities

### Verification
- [ ] Spawn enemies in level
- [ ] Enemies detect player and chase
- [ ] Enemies attack when in range
- [ ] Enemies take damage from player attacks
- [ ] Enemies die and despawn (or ragdoll)
- [ ] Multiple enemies at once

---

## Phase 8: Audio System (2-3 days)

### Audio Manager
- [ ] Create src/Engine/AudioManager.h/cpp
  - [ ] Initialize SDL_mixer or stb_vorbis
  - [ ] LoadSound(name, path)
  - [ ] PlaySound(name)
  - [ ] PlayMusic(path, loop)
  - [ ] StopMusic()
  - [ ] SetMasterVolume(volume)
  - [ ] SetSFXVolume(volume)
  - [ ] SetMusicVolume(volume)

### Sound Effects
- [ ] Download/generate player sounds:
  - [ ] Footsteps (stone floor)
  - [ ] Scythe whoosh (3 variations)
  - [ ] Shotgun fire
  - [ ] Reload click
  - [ ] Jump/land
  - [ ] Damage grunt
  - [ ] Rally recovery chime
- [ ] Download/generate enemy sounds:
  - [ ] Demon growls
  - [ ] Attack sounds
  - [ ] Death screams
- [ ] Download/generate environment sounds:
  - [ ] Wind ambience
  - [ ] Distant bells
  - [ ] Torch flicker

### Music
- [ ] Download dark ambient menu music
- [ ] Download intense combat music (gothic/horror)
- [ ] Optional: boss fight music

### Integration
- [ ] Hook sound effects to actions:
  - [ ] Player attacks
  - [ ] Player takes damage
  - [ ] Enemy attacks
  - [ ] Enemy deaths
  - [ ] Rally recovery
- [ ] Play background music in main menu
- [ ] Play combat music during gameplay
- [ ] Cross-fade between music tracks

### Verification
- [ ] Sound effects play on correct actions
- [ ] Background music loops seamlessly
- [ ] Volume controls adjust sound levels
- [ ] Multiple sounds can play simultaneously
- [ ] No audio popping or crackling

---

## Phase 9: UI & HUD (3-4 days)

### UI System
- [ ] Create src/Engine/UI.h/cpp
  - [ ] Orthographic projection for 2D
  - [ ] Render textured quads
  - [ ] Text rendering (bitmap font or SDL_ttf)
  - [ ] UI element positioning

### HUD Elements
- [ ] Health Bar
  - [ ] Create health_bar.png texture
  - [ ] Position: bottom-left
  - [ ] Show current/max health
  - [ ] Rally health (orange section)
  - [ ] Smooth transitions
- [ ] Ammo Counter
  - [ ] Create ammo_icon.png
  - [ ] Position: bottom-right
  - [ ] Show current / max ammo
  - [ ] Only visible in shotgun mode
  - [ ] Reload indicator
- [ ] Weapon Mode Indicator
  - [ ] Icon for long scythe mode
  - [ ] Icon for short scythe + shotgun mode
  - [ ] Visual feedback during switch
- [ ] Crosshair
  - [ ] Simple cross or dot
  - [ ] Center of screen
  - [ ] Color change when aiming at enemy
- [ ] Damage Numbers
  - [ ] Float upward from enemy on hit
  - [ ] Color coding (white, yellow critical, green rally)
  - [ ] Fade out over time

### Menus
- [ ] Main Menu
  - [ ] Background image (gothic theme)
  - [ ] New Game button
  - [ ] Settings button
  - [ ] Quit button
  - [ ] Keyboard/gamepad navigation
- [ ] Pause Menu
  - [ ] Resume button
  - [ ] Settings button
  - [ ] Main Menu button
- [ ] Settings Menu
  - [ ] Master volume slider
  - [ ] SFX volume slider
  - [ ] Music volume slider
  - [ ] Mouse sensitivity
  - [ ] Graphics quality (optional)

### Verification
- [ ] HUD displays correctly at all resolutions
- [ ] Health bar updates in real-time
- [ ] Rally health indicator works
- [ ] Ammo counter shows correct values
- [ ] Damage numbers appear on hits
- [ ] Menus navigable with keyboard and gamepad
- [ ] Settings changes apply immediately

---

## Phase 10: Advanced Physics - Future (6-8 days)

> Note: This phase is optional and for future enhancement

### Physics Engine Integration
- [ ] Choose physics library (JoltPhysics recommended)
- [ ] Download and integrate into project
- [ ] Initialize physics world
- [ ] Create physics update loop
- [ ] Sync physics with rendering

### Cloth Simulation for Player
- [ ] Create ClothSimulation class
  - [ ] Grid of particles
  - [ ] Structural constraints (horizontal/vertical)
  - [ ] Shear constraints (diagonal)
  - [ ] Bending constraints
- [ ] Implement Verlet integration
  - [ ] Update particle positions
  - [ ] Apply forces (gravity, wind)
  - [ ] Satisfy constraints iteratively
- [ ] Pin cloth to player skeleton
  - [ ] Top particles follow shoulder bones
  - [ ] Rest of cloth simulates freely
- [ ] Cloth collision
  - [ ] Self-collision
  - [ ] Player body collision
  - [ ] Environment collision
- [ ] Render cloth mesh
  - [ ] Generate triangles from particle grid
  - [ ] Calculate normals
  - [ ] Apply cloth texture

### Ragdoll Physics for Enemies
- [ ] Create Physics Asset for enemy skeleton
  - [ ] Capsule for torso
  - [ ] Capsules for limbs
  - [ ] Sphere for head
- [ ] Define joint constraints
  - [ ] Hinge joints (elbows, knees)
  - [ ] Cone-twist (shoulders, hips)
  - [ ] Angular limits
- [ ] Implement Enemy::Die() ragdoll activation
  - [ ] Disable animation
  - [ ] Enable physics simulation
  - [ ] Apply death impulse
- [ ] Update skeleton from physics
  - [ ] Read rigid body transforms
  - [ ] Apply to bones
- [ ] Fine-tune:
  - [ ] Mass distribution
  - [ ] Damping
  - [ ] Friction
  - [ ] Joint limits

### Verification
- [ ] Player cape/coat flows naturally
- [ ] Cloth responds to movement and jumps
- [ ] Cloth collides with player body
- [ ] Enemies ragdoll on death
- [ ] Ragdoll reacts to killing blow direction
- [ ] No jittering or instability
- [ ] Performance remains at 60 FPS

---

## Asset Acquisition Checklist

### 3D Models
- [ ] Player character (gothic hunter with coat/cape)
- [ ] Long scythe (full-length, two-handed)
- [ ] Short scythe blade (one-handed)
- [ ] Vintage double-barrel shotgun
- [ ] Lesser demon (low-poly)
- [ ] Possessed cultist (humanoid)
- [ ] Gargoyle (winged)
- [ ] Hellhound (quadruped)
- [ ] Gothic pillar
- [ ] Gargoyle statue
- [ ] Tombstone (3 variations)
- [ ] Cemetery gate
- [ ] Church altar
- [ ] Church pew
- [ ] Stained glass window

### Textures
- [ ] Gothic stone (dark, weathered)
- [ ] Stained glass pattern
- [ ] Dark wood (aged)
- [ ] Rusted metal
- [ ] Demon skin (scales/organic)
- [ ] Blood decal
- [ ] Grass (dead, withered)

### Audio - SFX
- [ ] Scythe whoosh (3 variations)
- [ ] Shotgun blast
- [ ] Shotgun reload
- [ ] Footsteps on stone (4 variations)
- [ ] Jump/land sounds
- [ ] Player damage grunt (3 variations)
- [ ] Rally recovery chime
- [ ] Demon growl (idle)
- [ ] Demon attack sound
- [ ] Demon death scream
- [ ] Cultist chant
- [ ] Wind ambience
- [ ] Church bell (distant)
- [ ] Door creak
- [ ] Torch crackle

### Audio - Music
- [ ] Main menu theme (dark ambient, organ)
- [ ] Combat theme (intense, gothic choir)
- [ ] Boss fight theme (epic orchestral horror)
- [ ] Victory jingle (somber)

### UI Assets
- [ ] Health bar frame
- [ ] Health bar fill (red)
- [ ] Rally health fill (orange)
- [ ] Ammo icon (shotgun shell)
- [ ] Long scythe icon
- [ ] Short scythe + shotgun icon
- [ ] Crosshair (simple cross)
- [ ] Damage number font
- [ ] Menu background (gothic cathedral)
- [ ] Button texture (stone/metal)

---

## TrenchBroom Setup Checklist

- [ ] Download TrenchBroom from https://trenchbroom.github.io/
- [ ] Create `VibeReaper/assets/game.fgd` file
- [ ] Define baseclass for all entities
- [ ] Define info_player_start entity
- [ ] Define light entity (color, intensity properties)
- [ ] Define enemy_demon entity
- [ ] Define enemy_cultist entity
- [ ] Define enemy_gargoyle entity (if implemented)
- [ ] Define enemy_hellhound entity (if implemented)
- [ ] Configure TrenchBroom game preferences
- [ ] Set game path to VibeReaper project
- [ ] Set FGD path to assets/game.fgd
- [ ] Create first test map: test_room.map
- [ ] Export and load in game successfully

---

## Current Status

**Phase**: Phase 4 Complete - Ready for Phase 5
**Status**: Player controller, input system, and camera following fully implemented
**Last Updated**: 2025-11-30

### Recent Accomplishments

**Phase 1 - Engine Foundation** ✅
- Complete build system with CMake
- SDL2 + OpenGL initialization
- Main game loop with delta time
- Renderer, Shader, and Logger systems

**Phase 2 - 3D Rendering** ✅
- Mesh system with VAO/VBO/EBO
- Texture loading with stb_image
- Third-person orbital camera
- Phong lighting shader
- Code review (Grade: A-)
- Test suite (12 tests)

**Phase 3 - TrenchBroom Integration** ✅
- Quake MAP format parser
- Brush-to-mesh conversion with plane intersection
- Texture coordinate transformations (scale, rotation, offset)
- Pixel-perfect texture filtering (GL_NEAREST)
- World entity system (player spawn, lights)
- VS Code build configuration
- Custom VibeReaper.fgd file
- Test map rendering successfully

**Phase 4 - Player Controller & Input** ✅
- Unified input system (keyboard/mouse + gamepad)
- Player movement with WASD and left stick
- Camera rotation with mouse and right stick
- Smooth camera following with lerp
- Player physics (gravity, jumping)
- Player rendering with placeholder mesh
- AABB collision system (framework ready)
- Movement feel tuning complete

### Next Steps: Phase 5 - Combat System

**Objectives**:
1. Base weapon class
2. Scythe weapon (dual mode - long/short)
3. Shotgun weapon
4. Enemy base class
5. Basic combat mechanics

**Estimated Duration**: 5-6 days
**Priority Systems**: Weapon → Enemy → Combat Loop

### Technical Notes

- Input system supports hot-plugging gamepads
- Player movement uses smooth acceleration/deceleration
- Camera follows player with configurable distance/rotation
- Collision framework ready for world integration
- Player spawns at info_player_start from MAP file
- Movement speed: 6.0 units/sec, jump strength: 8.0, gravity: -20.0

---

This checklist will be updated as development progresses. Check off items as they are completed.
