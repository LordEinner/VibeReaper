#pragma once

namespace VibeReaper {

    /**
     * @brief Global engine constants
     *
     * MAP_UNITS_PER_METER: Conversion ratio between Quake MAP units and real-world meters
     * - Default: 64 units = 1 meter
     * - Adjust this value if a different scale feels more appropriate during development
     * - Used for player size, movement speed, camera distance, and all physical measurements
     */
    constexpr float MAP_UNITS_PER_METER = 64.0f;

    /**
     * @brief User-defined literal for MAP units (meters)
     *
     * Provides elegant syntax for converting real-world meters to MAP units.
     * The '_u' suffix stands for "units" (MAP units).
     *
     * Usage examples:
     * - Player height: 1.75_u    → 112.0 MAP units (1.75m × 64)
     * - Camera distance: 5.0_u   → 320.0 MAP units (5m × 64)
     * - Movement speed: 5.5_u    → 352.0 MAP units (5.5m × 64)
     *
     * The literal automatically scales by MAP_UNITS_PER_METER, so changing
     * the scale constant will update all measurements using _u throughout the codebase.
     *
     * @param meters Real-world measurement in meters
     * @return Equivalent value in MAP units
     */
    constexpr float operator"" _u(long double meters) {
        return static_cast<float>(meters) * MAP_UNITS_PER_METER;
    }

    constexpr float operator"" _u(unsigned long long meters) {
        return static_cast<float>(meters) * MAP_UNITS_PER_METER;
    }

} // namespace VibeReaper
