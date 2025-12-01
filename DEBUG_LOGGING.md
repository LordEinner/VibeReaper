# Debug Logging Instructions

## How to Use Debug Logging

When running VibeReaper.exe, you can capture detailed camera collision debug information:

1. **Run the game** from `build/bin/VibeReaper.exe`

2. **During gameplay**, press:
   - **Q key** - Log debug info when camera is **working correctly**
   - **E key** - Log debug info when camera is **working incorrectly**

3. **Find the log file**:
   - Location: `build/bin/VibeReaper.log`
   - The log file is created in the same directory as the executable

## What Gets Logged

The debug log includes:
- Camera state (position, target, yaw, pitch)
- Distance values (desired, current, actual)
- Camera direction vector
- Nearby geometry analysis:
  - AABB bounds and center
  - Distance from player
  - Direction alignment (dot product)
  - Raycast hit results

## Analyzing the Logs

Compare "working correctly" entries with "working incorrectly" entries to identify:
- Which geometry is causing false collisions
- Direction alignment issues
- Distance calculation problems
- Raycast hit discrepancies

## Log File Location

The log file is created fresh each time you run the game (truncate mode).
Previous logs are overwritten, so save them if needed before rerunning.
