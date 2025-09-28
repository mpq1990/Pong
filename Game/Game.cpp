#include "Game.h"

Game::Game() noexcept = default;

bool Game::Initialize() noexcept {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("Game Programming in C++",
        kScreenWidth, kScreenHeight, 0);
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, nullptr);
    if (!mRenderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
        return false;
    }

    // Enable vsync if possible (SDL3: 0 = success, -1 = failure)
    if (!SDL_SetRenderVSync(mRenderer, 1)) {
        SDL_Log("Warning: VSync not supported: %s", SDL_GetError());
    }

    // Initial positions (same as your original)
    mPaddlePos = { 10.0f, static_cast<float>(kScreenHeight) / 2.0f };
    mBallPos = { static_cast<float>(kScreenWidth) / 2.0f,
                   static_cast<float>(kScreenHeight) / 2.0f };

    mIsRunning = true;
    mPaddleDir = 0;
    mTicksCount = SDL_GetTicks(); // Uint64

    return true;
}

void Game::Shutdown() noexcept {
    if (mRenderer) {
        SDL_DestroyRenderer(mRenderer);
        mRenderer = nullptr;
    }
    if (mWindow) {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }
    SDL_Quit();
}

// ---------------------------
// main game loop
// ---------------------------
void Game::RunLoop() noexcept {
    while (mIsRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

// ---------------------------
// input
// ---------------------------
void Game::ProcessInput() noexcept {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            mIsRunning = false;
        }
    }

    const bool* state = SDL_GetKeyboardState(nullptr);

    if (state[SDL_SCANCODE_ESCAPE]) {
        mIsRunning = false;
    }

    mPaddleDir = 0;
    if (state[SDL_SCANCODE_W]) mPaddleDir -= 1;
    if (state[SDL_SCANCODE_S]) mPaddleDir += 1;
}

// ---------------------------
// update
// ---------------------------
void Game::UpdateGame() noexcept {
    // Busy-wait cap to ~60 FPS (kept to preserve your behavior)
    while (SDL_GetTicks() < mTicksCount + kFrameMS) { /* spin */ }

    float deltaTime = static_cast<float>(SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > kMaxDelta) {
        deltaTime = kMaxDelta;
    }

    // Move paddle (unchanged logic)
    if (mPaddleDir != 0) {
        mPaddlePos.y += mPaddleDir * kPaddleSpeed * deltaTime;

        const float topLimit =
            (kPaddleHeight / 2.0f) + static_cast<float>(kThickness);
        const float bottomLimit =
            static_cast<float>(kScreenHeight - kThickness) - (kPaddleHeight / 2.0f);

        if (mPaddlePos.y < topLimit) {
            mPaddlePos.y = topLimit;
        }
        else if (mPaddlePos.y > bottomLimit) {
            mPaddlePos.y = bottomLimit;
        }
    }

    // Move ball
    mBallPos.x += mBallVelocity.x * deltaTime;
    mBallPos.y += mBallVelocity.y * deltaTime;

    // Top wall
    if (mBallPos.y <= static_cast<float>(kThickness) && mBallVelocity.y < 0.0f) {
        mBallVelocity.y *= -1.0f;
    }
    // Bottom wall
    if (mBallPos.y >= static_cast<float>(kScreenHeight - kThickness) && mBallVelocity.y > 0.0f) {
        mBallVelocity.y *= -1.0f;
    }
    // Right wall
    if (mBallPos.x >= static_cast<float>(kScreenWidth - kThickness) && mBallVelocity.x > 0.0f) {
        mBallVelocity.x *= -1.0f;
    }

    // Paddle collision
    float diff = mPaddlePos.y - mBallPos.y;
    diff = (diff > 0.0f) ? diff : -diff;
    if (diff <= (kPaddleHeight / 2.0f) &&
        mBallVelocity.x < 0.0f &&
        mBallPos.x <= 25.0f && mBallPos.x >= 20.0f) {
        mBallVelocity.x *= -1.0f;
    }
    else if (mBallPos.x <= 0.0f) {
        mIsRunning = false; // fixed stray comma from original
    }

    mTicksCount = SDL_GetTicks(); // Uint64
}

// ---------------------------
// render
// ---------------------------
void Game::GenerateOutput() noexcept {
    // Clear (blue)
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);
    SDL_RenderClear(mRenderer);

    // Draw color (white)
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

    // Top wall
    SDL_FRect wall{
        0.0f, 0.0f,
        static_cast<float>(kScreenWidth),
        static_cast<float>(kThickness)
    };
    SDL_RenderFillRect(mRenderer, &wall);

    // Bottom wall
    wall.y = static_cast<float>(kScreenHeight - kThickness);
    SDL_RenderFillRect(mRenderer, &wall);

    // Right wall (kept your original wall.h = 1024.0f on purpose)
    wall.x = static_cast<float>(kScreenWidth - kThickness);
    wall.y = 0.0f;
    wall.h = 1024.0f;
    wall.w = static_cast<float>(kThickness);
    SDL_RenderFillRect(mRenderer, &wall);

    // Paddle
    SDL_FRect paddle{
        mPaddlePos.x - (static_cast<float>(kThickness) / 2.0f),
        mPaddlePos.y - (kPaddleHeight / 2.0f),
        static_cast<float>(kThickness),
        kPaddleHeight
    };
    SDL_RenderFillRect(mRenderer, &paddle);

    // Ball
    SDL_FRect ball{
        mBallPos.x - (static_cast<float>(kThickness) / 2.0f),
        mBallPos.y - (static_cast<float>(kThickness) / 2.0f),
        static_cast<float>(kThickness),
        static_cast<float>(kThickness)
    };
    SDL_RenderFillRect(mRenderer, &ball);

    // Present
    SDL_RenderPresent(mRenderer);
}
