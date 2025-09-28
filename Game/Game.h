#pragma once
#include <SDL3/SDL.h>

struct Vector2 {
    float x{};
    float y{};
};

class Game {
public:
    Game() noexcept;

    [[nodiscard]] bool Initialize() noexcept;
    void RunLoop() noexcept;
    void Shutdown() noexcept;

private:
    void ProcessInput() noexcept;
    void UpdateGame() noexcept;
    void GenerateOutput() noexcept;

    // Constants (names only — gameplay/values unchanged)
    static constexpr int    kScreenWidth = 1024;
    static constexpr int    kScreenHeight = 768;
    static constexpr int    kThickness = 15;
    static constexpr float  kPaddleHeight = 100.0f;
    static constexpr float  kPaddleSpeed = 300.0f;
    static constexpr float  kMaxDelta = 0.05f;
    static constexpr Uint64 kFrameMS = 16;   // match SDL_GetTicks() type

    // SDL resources
    SDL_Window* mWindow{ nullptr };
    SDL_Renderer* mRenderer{ nullptr };

    // Game state
    Vector2 mPaddlePos{};
    Vector2 mBallPos{};
    Vector2 mBallVelocity{ -200.0f, 235.0f };

    bool    mIsRunning{ true };
    Uint64  mTicksCount{ 0 };  
    int     mPaddleDir{ 0 };    
};
