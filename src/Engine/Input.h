#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <unordered_map>

namespace VibeReaper {

    /**
     * @brief Unified input system supporting keyboard, mouse, and gamepad
     *
     * Provides abstraction for all input types with support for:
     * - Continuous hold detection (IsKeyPressed)
     * - Single-frame press detection (IsKeyJustPressed)
     * - Mouse delta tracking
     * - Gamepad analog stick input with deadzone
     * - Hot-plugging gamepad support
     */
    class Input {
    public:
        Input();
        ~Input();

        // Disable copy/move (singleton pattern)
        Input(const Input&) = delete;
        Input& operator=(const Input&) = delete;

        /**
         * @brief Process SDL events - call this in main event loop
         * @param event SDL_Event to process
         */
        void ProcessEvent(const SDL_Event& event);

        /**
         * @brief Prepare input state - call once BEFORE event loop
         * Captures current state to previous state before SDL updates it
         */
        void Prepare();

        /**
         * @brief Update input state - call once AFTER event loop
         */
        void Update();

        // Keyboard
        bool IsKeyPressed(SDL_Scancode key) const;
        bool IsKeyJustPressed(SDL_Scancode key) const;

        // Mouse
        glm::vec2 GetMouseDelta() const;
        bool IsMouseButtonPressed(int button) const;
        bool IsMouseButtonJustPressed(int button) const;

        // Gamepad
        bool IsGamepadConnected() const;
        float GetAxis(SDL_GameControllerAxis axis) const;
        bool IsButtonPressed(SDL_GameControllerButton button) const;
        bool IsButtonJustPressed(SDL_GameControllerButton button) const;

        // Mouse capture (for FPS-style camera control)
        void SetMouseCaptured(bool captured);
        bool IsMouseCaptured() const { return mouseCaptured; }

        // Camera invert settings
        void SetInvertHorizontal(bool invert) { invertHorizontal = invert; }
        void SetInvertVertical(bool invert) { invertVertical = invert; }
        bool GetInvertHorizontal() const { return invertHorizontal; }
        bool GetInvertVertical() const { return invertVertical; }

    private:
        // Keyboard state
        const Uint8* currentKeyState;
        Uint8* previousKeyState;
        int keyStateLength;

        // Mouse state
        glm::vec2 mouseDelta;
        Uint32 currentMouseState;
        Uint32 previousMouseState;
        bool mouseCaptured;

        // Camera settings
        bool invertHorizontal;
        bool invertVertical;

        // Gamepad state
        SDL_GameController* gamepad;
        std::unordered_map<SDL_GameControllerButton, bool> currentButtonState;
        std::unordered_map<SDL_GameControllerButton, bool> previousButtonState;

        // Constants
        static constexpr float DEADZONE = 0.15f;

        // Helpers
        void OpenGamepad(int deviceIndex);
        void CloseGamepad();
        float ApplyDeadzone(float value) const;
    };

} // namespace VibeReaper
