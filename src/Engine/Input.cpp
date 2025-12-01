#include "Input.h"
#include "../Utils/Logger.h"
#include <cstring>
#include <algorithm>

namespace VibeReaper {

    Input::Input()
        : currentKeyState(nullptr),
          previousKeyState(nullptr),
          keyStateLength(0),
          mouseDelta(0.0f, 0.0f),
          currentMouseState(0),
          previousMouseState(0),
          mouseCaptured(false),
          invertHorizontal(true),   // Invert horizontal by default (standard for most games)
          invertVertical(false),    // Don't invert vertical by default (non-inverted is more common for TPS)
          gamepad(nullptr) {

        // Initialize keyboard state
        currentKeyState = SDL_GetKeyboardState(&keyStateLength);
        previousKeyState = new Uint8[keyStateLength];
        std::memset(previousKeyState, 0, keyStateLength);

        // Try to open first gamepad
        for (int i = 0; i < SDL_NumJoysticks(); ++i) {
            if (SDL_IsGameController(i)) {
                OpenGamepad(i);
                break;
            }
        }

        LOG_INFO("Input system initialized");
    }

    Input::~Input() {
        delete[] previousKeyState;
        CloseGamepad();
    }

    void Input::ProcessEvent(const SDL_Event& event) {
        switch (event.type) {
            case SDL_CONTROLLERDEVICEADDED:
                if (!gamepad) {
                    OpenGamepad(event.cdevice.which);
                }
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                if (gamepad && event.cdevice.which == SDL_JoystickInstanceID(
                    SDL_GameControllerGetJoystick(gamepad))) {
                    CloseGamepad();
                }
                break;

            case SDL_CONTROLLERBUTTONDOWN:
                if (gamepad) {
                    currentButtonState[static_cast<SDL_GameControllerButton>(event.cbutton.button)] = true;
                }
                break;

            case SDL_CONTROLLERBUTTONUP:
                if (gamepad) {
                    currentButtonState[static_cast<SDL_GameControllerButton>(event.cbutton.button)] = false;
                }
                break;

            default:
                break;
        }
    }

    void Input::Prepare() {
        // Update keyboard state - MUST be done before SDL_PollEvent
        // SDL_PollEvent updates the internal array returned by SDL_GetKeyboardState
        // so we must copy it to previous BEFORE processing events
        std::memcpy(previousKeyState, currentKeyState, keyStateLength);
    }

    void Input::Update() {
        // Keyboard state is already prepared in Prepare()
        // Just ensure current pointer is valid (it points to internal SDL memory)
        currentKeyState = SDL_GetKeyboardState(nullptr);

        // Update mouse state
        previousMouseState = currentMouseState;
        int mouseX, mouseY;
        if (mouseCaptured) {
            currentMouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
            mouseDelta = glm::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
        } else {
            currentMouseState = SDL_GetMouseState(&mouseX, &mouseY);
            mouseDelta = glm::vec2(0.0f, 0.0f);
        }

        // Update gamepad button state
        previousButtonState = currentButtonState;
    }

    bool Input::IsKeyPressed(SDL_Scancode key) const {
        return currentKeyState[key] != 0;
    }

    bool Input::IsKeyJustPressed(SDL_Scancode key) const {
        return currentKeyState[key] != 0 && previousKeyState[key] == 0;
    }

    glm::vec2 Input::GetMouseDelta() const {
        return mouseDelta;
    }

    bool Input::IsMouseButtonPressed(int button) const {
        return (currentMouseState & SDL_BUTTON(button)) != 0;
    }

    bool Input::IsMouseButtonJustPressed(int button) const {
        return (currentMouseState & SDL_BUTTON(button)) != 0 &&
               (previousMouseState & SDL_BUTTON(button)) == 0;
    }

    bool Input::IsGamepadConnected() const {
        return gamepad != nullptr;
    }

    float Input::GetAxis(SDL_GameControllerAxis axis) const {
        if (!gamepad) return 0.0f;

        Sint16 value = SDL_GameControllerGetAxis(gamepad, axis);
        float normalized = value / 32767.0f;
        return ApplyDeadzone(normalized);
    }

    bool Input::IsButtonPressed(SDL_GameControllerButton button) const {
        if (!gamepad) return false;

        auto it = currentButtonState.find(button);
        return it != currentButtonState.end() && it->second;
    }

    bool Input::IsButtonJustPressed(SDL_GameControllerButton button) const {
        if (!gamepad) return false;

        auto currentIt = currentButtonState.find(button);
        auto previousIt = previousButtonState.find(button);

        bool currentPressed = currentIt != currentButtonState.end() && currentIt->second;
        bool previousPressed = previousIt != previousButtonState.end() && previousIt->second;

        return currentPressed && !previousPressed;
    }

    void Input::SetMouseCaptured(bool captured) {
        mouseCaptured = captured;
        SDL_SetRelativeMouseMode(captured ? SDL_TRUE : SDL_FALSE);
        if (captured) {
            LOG_INFO("Mouse captured");
        } else {
            LOG_INFO("Mouse released");
        }
    }

    void Input::OpenGamepad(int deviceIndex) {
        gamepad = SDL_GameControllerOpen(deviceIndex);
        if (gamepad) {
            const char* name = SDL_GameControllerName(gamepad);
            LOG_INFO("Gamepad connected: " + std::string(name ? name : "Unknown"));
        } else {
            LOG_ERROR("Failed to open gamepad: " + std::string(SDL_GetError()));
        }
    }

    void Input::CloseGamepad() {
        if (gamepad) {
            SDL_GameControllerClose(gamepad);
            gamepad = nullptr;
            currentButtonState.clear();
            previousButtonState.clear();
            LOG_INFO("Gamepad disconnected");
        }
    }

    float Input::ApplyDeadzone(float value) const {
        if (std::abs(value) < DEADZONE) {
            return 0.0f;
        }

        // Rescale value to account for deadzone
        float sign = value > 0.0f ? 1.0f : -1.0f;
        float magnitude = std::abs(value);
        return sign * ((magnitude - DEADZONE) / (1.0f - DEADZONE));
    }

} // namespace VibeReaper
