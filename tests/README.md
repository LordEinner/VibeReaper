# VibeReaper Test Suite

This directory contains automated tests for the VibeReaper engine components.

## Building Tests

To build the test suite, enable the `BUILD_TESTS` option when configuring CMake:

```bash
# Windows (MinGW)
cmake -B build -DBUILD_TESTS=ON
cmake --build build --config Release

# Run tests
build/bin/VibeReaperTests.exe
```

```bash
# Linux
cmake -B build -DBUILD_TESTS=ON
make -C build

# Run tests
./build/bin/VibeReaperTests
```

## Test Coverage

### Unit Tests (No GPU Required)

These tests run without requiring an OpenGL context:

1. **Mesh: Vertex Structure**
   - Tests default vertex initialization
   - Tests vertex constructor with parameters
   - Verifies position, normal, and texCoord fields

2. **Mesh: Cube Generation**
   - Verifies 24 vertices (4 per face)
   - Verifies 36 indices (6 faces × 2 triangles × 3 vertices)
   - Checks all normals are unit vectors
   - Validates UV coordinates are in [0, 1] range
   - Ensures indices are within valid range

3. **Mesh: Sphere Generation**
   - Verifies vertices exist and form complete triangles
   - Checks vertices lie on sphere surface (radius 0.5)
   - Validates normals point outward from center
   - Tests with different subdivision levels

4. **Mesh: Plane Generation**
   - Verifies 4 vertices and 6 indices
   - Checks all vertices lie on Y=0 plane
   - Validates normals point upward (0, 1, 0)
   - Tests custom width and depth parameters

5. **Camera: Initialization**
   - Tests default target at origin
   - Verifies default yaw, pitch, distance values
   - Checks FOV and aspect ratio settings

6. **Camera: Orbit Controls**
   - Tests yaw and pitch modification
   - Verifies position changes after orbit
   - Tests pitch clamping at ±89° (gimbal lock prevention)
   - Validates spherical coordinate conversion

7. **Camera: Zoom Controls**
   - Tests distance modification
   - Verifies minimum distance clamping (1.0)
   - Verifies maximum distance clamping (50.0)

8. **Camera: Matrix Generation**
   - Ensures view matrix is non-zero and non-identity
   - Ensures projection matrix is non-zero and non-identity
   - Validates matrices are properly generated

9. **Camera: Spherical Coordinate Conversion**
   - Tests conversion from spherical to Cartesian coordinates
   - Validates position calculation for cardinal directions

### Integration Tests (GPU Required)

These tests require an OpenGL context:

10. **Mesh: GPU Buffer Setup**
    - Creates mesh and verifies GPU buffers are generated
    - Ensures VAO/VBO/EBO are properly initialized
    - Tests mesh can be drawn without errors

11. **Texture: Loading**
    - Tests loading texture from file
    - Verifies texture dimensions and channel count
    - Checks OpenGL texture ID is valid
    - Validates mipmap generation

12. **Shader: Compilation**
    - Tests loading vertex and fragment shaders
    - Verifies shader program compilation and linking
    - Tests uniform setters (int, float, vec3, mat4)
    - Ensures no OpenGL errors during shader usage

## Expected Results

When all tests pass, you should see:

```
========================================
  VibeReaper Phase 2 Test Suite
========================================

--- UNIT TESTS (No GPU Required) ---

[TEST] Mesh: Vertex Structure...
  ✓ PASSED

[TEST] Mesh: Cube Generation...
  ✓ PASSED

[TEST] Mesh: Sphere Generation...
  ✓ PASSED

[TEST] Mesh: Plane Generation...
  ✓ PASSED

[TEST] Camera: Initialization...
  ✓ PASSED

[TEST] Camera: Orbit Controls...
  ✓ PASSED

[TEST] Camera: Zoom Controls...
  ✓ PASSED

[TEST] Camera: Matrix Generation...
  ✓ PASSED

[TEST] Camera: Spherical Coordinate Conversion...
  ✓ PASSED

--- INTEGRATION TESTS (GPU Required) ---
OpenGL Version: <Your GPU OpenGL Version>

[TEST] Mesh: GPU Buffer Setup...
  ✓ PASSED

[TEST] Texture: Loading (requires test texture)...
  ✓ PASSED

[TEST] Shader: Compilation (requires shader files)...
  ✓ PASSED

========================================
  TEST RESULTS
========================================
Passed: 12
Failed: 0
Total:  12

✓ ALL TESTS PASSED!
```

## Troubleshooting

### Tests Skipped (Warnings)

Some integration tests may show warnings if assets are not found:

```
⚠ WARNING: Test texture not found (acceptable for unit test)
⚠ WARNING: Shader files not found (acceptable for unit test)
```

This is expected if you run tests from a directory without the `assets/` folder. The test suite will skip these tests gracefully.

### SDL/OpenGL Initialization Failed

If SDL or OpenGL initialization fails:

```
⚠ WARNING: Could not initialize SDL. Skipping GPU tests.
⚠ WARNING: Could not create OpenGL context. Skipping GPU tests.
```

This can happen in headless environments or if GPU drivers are not installed. Unit tests will still run successfully.

### Build Errors

**Missing stb_image.h:**
Ensure `lib/stb/stb_image.h` exists. Download from: https://github.com/nothings/stb

**Linking errors:**
Ensure SDL2 libraries are properly installed and `SDL2_PATH` in CMakeLists.txt points to the correct location.

## Adding New Tests

To add a new test:

1. Create a test function following the pattern:
   ```cpp
   bool test_your_feature() {
       TEST_START("Category: Feature Name");

       // Your test code here
       TEST_ASSERT(condition, "Assertion message");

       TEST_PASS();
   }
   ```

2. Call the function from `main()` in the appropriate section (unit tests or integration tests)

3. Rebuild with `-DBUILD_TESTS=ON`

## Test Framework

The test suite uses a minimal custom test framework with these macros:

- `TEST_START(name)`: Begin a test
- `TEST_ASSERT(condition, message)`: Assert a condition
- `TEST_PASS()`: Mark test as passed

Test results are tracked globally in `tests_passed` and `tests_failed` counters.

## Continuous Integration

For CI/CD pipelines, the test executable returns:
- **Exit code 0**: All tests passed
- **Exit code 1**: One or more tests failed

Example CI usage:
```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build --config Release
./build/bin/VibeReaperTests || exit 1
```

## Future Enhancements

- [ ] Add performance benchmarks
- [ ] Add stress tests (large meshes, many textures)
- [ ] Add memory leak detection (Valgrind/Dr. Memory)
- [ ] Add code coverage reporting
- [ ] Integrate with test frameworks (Catch2, Google Test)
- [ ] Add visual regression tests (screenshot comparison)
