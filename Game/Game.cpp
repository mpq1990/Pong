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

    // Enable vsync if possible
    if (!SDL_SetRenderVSync(mRenderer, 1)) {
        SDL_Log("Warning: VSync not supported: %s", SDL_GetError());
    }

    // Initial positions
    mLeftPaddlePos = { 10.0f, static_cast<float>(kScreenHeight) / 2.0f };
    mRightPaddlePos = { 1024.0f - 10.0f, static_cast<float>(kScreenHeight) / 2.0f };
    mBallPos = { static_cast<float>(kScreenWidth) / 2.0f,
                   static_cast<float>(kScreenHeight) / 2.0f };

    mIsRunning = true;
    mLeftPaddleDir = 0;
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

    mLeftPaddleDir = 0;
    if (state[SDL_SCANCODE_W]) mLeftPaddleDir -= 1;
    if (state[SDL_SCANCODE_S]) mLeftPaddleDir += 1;

    mRightPaddleDir = 0;
    if (state[SDL_SCANCODE_I]) mRightPaddleDir -= 1;
    if (state[SDL_SCANCODE_K]) mRightPaddleDir += 1;
}

void Game::MovePaddle(int paddleDir, Vector2& paddlePos, float deltaTime) noexcept {
    if (paddleDir != 0) {
        paddlePos.y += paddleDir * kPaddleSpeed * deltaTime;

        const float topLimit =
            (kPaddleHeight / 2.0f) + static_cast<float>(kThickness);
        const float bottomLimit =
            static_cast<float>(kScreenHeight - kThickness) - (kPaddleHeight / 2.0f);

        if (paddlePos.y < topLimit) {
            paddlePos.y = topLimit;
        }
        else if (paddlePos.y > bottomLimit) {
            paddlePos.y = bottomLimit;
        }
    }
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

    // Move Left paddle 
    MovePaddle(mLeftPaddleDir, mLeftPaddlePos, deltaTime);

    // Move Right paddle
    MovePaddle(mRightPaddleDir, mRightPaddlePos, deltaTime);

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

    // Commenting out for two players
    // Right wall
    //if (mBallPos.x >= static_cast<float>(kScreenWidth - kThickness) && mBallVelocity.x > 0.0f) {
    //    mBallVelocity.x *= -1.0f;
    //}

     //Left Paddle collision
     float diff = mLeftPaddlePos.y - mBallPos.y;
     diff = (diff > 0.0f) ? diff : -diff;
     if (!CheckPaddleCollision(mLeftPaddlePos, diff, true) &&
         mBallPos.x <= 0.0f) {
         mIsRunning = false;
     }

    // Right Paddle collistion
     float rightDiff = mRightPaddlePos.y - mBallPos.y;
     rightDiff = (rightDiff > 0.0f) ? rightDiff : -rightDiff;
     if (!CheckPaddleCollision(mRightPaddlePos, rightDiff, false) &&
         mBallPos.x >= kScreenWidth) {
         mIsRunning = false;
     }

    mTicksCount = SDL_GetTicks(); // Uint64
}

bool Game::CheckPaddleCollision(const Vector2& paddlePos,
    float diff,
    bool isLeftPaddle) noexcept {
    if (diff <= (kPaddleHeight / 2.0f)) {
        if (isLeftPaddle) {
            if (mBallVelocity.x < 0.0f &&
                mBallPos.x <= 25.0f && mBallPos.x >= 20.0f) {
                mBallVelocity.x *= -1.0f;
                return true;
            }
        }
        else { // right paddle
            if (mBallVelocity.x > 0.0f &&
                mBallPos.x >= (kScreenWidth - 25.0f) &&
                mBallPos.x <= (kScreenWidth - 20.0f)) {
                mBallVelocity.x *= -1.0f;
                return true;
            }
        }
    }
    return false;
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

    // Commenting out for multiplayer
    //// Right wall
    //wall.x = static_cast<float>(kScreenWidth - kThickness);
    //wall.y = 0.0f;
    //wall.h = kScreenHeight;
    //wall.w = static_cast<float>(kThickness);
    //SDL_RenderFillRect(mRenderer, &wall);

    // Left Paddle
    SDL_FRect leftPaddle{
        mLeftPaddlePos.x - (static_cast<float>(kThickness) / 2.0f),
        mLeftPaddlePos.y - (kPaddleHeight / 2.0f),
        static_cast<float>(kThickness),
        kPaddleHeight
    };
    SDL_RenderFillRect(mRenderer, &leftPaddle);

    // Right Paddle
    SDL_FRect rightPaddle{
        mRightPaddlePos.x - (static_cast<float>(kThickness) / 2.0f),
        mRightPaddlePos.y - (kPaddleHeight / 2.0f),
        static_cast<float>(kThickness),
        kPaddleHeight
    };
    SDL_RenderFillRect(mRenderer, &rightPaddle);

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
