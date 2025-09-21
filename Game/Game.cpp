#include "Game.h"
#include <SDL3/SDL.h>

static constexpr int thickness = 15;
static constexpr float paddelHeight = 100.0f;

Game::Game() :mWindow{nullptr }, mRenderer{ nullptr }, mIsRunning{ true } {}

// Initialises the Game
bool Game::Initialize() {
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (!sdlResult) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow("Game Programming in C++", 1024, 768, 0);
	if (!mWindow) {
		SDL_Log("Failed to create windows: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(mWindow, nullptr);
	if (!mRenderer) {
		SDL_Log("Unable to create renderer: %s", SDL_GetError());
		return false;
	}

	int vSync = SDL_SetRenderVSync(mRenderer, 1); // can inline, but for readability its fine for now
	if (!vSync) {
		SDL_Log("Warning: VSync not supported: %s", SDL_GetError());

	}

	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f / 2.0f;
	mBallPos.x = 1024.0f / 2.0f;
	mBallPos.y = 768.0f / 2.0f;


	mIsRunning = true;
	return true;
}


// Terminates the Game
void Game::Shutdown() {
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

// Main Game Loop
void Game::RunLoop() {
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_EVENT_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const bool* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE]) {
		mIsRunning = false;
	}
};

void Game::UpdateGame() {};

void Game::GenerateOutput() {
	// Set draw color to blue
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);

	// clear back buffer
	SDL_RenderClear(mRenderer);

	// draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	// top wall
	SDL_FRect wall{
		0,
		0,
		1024,
		thickness
	};

	SDL_RenderFillRect(mRenderer, &wall);

	//bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	// right wall
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.h = 1024;
	wall.w = thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	// draw paddel
	SDL_FRect paddel{
		mPaddlePos.x - (thickness / 2),
		mPaddlePos.y - (paddelHeight / 2),
		thickness,
		paddelHeight
	};
	SDL_RenderFillRect(mRenderer, &paddel);

	// draw ball
	SDL_FRect ball{
		mBallPos.x - (thickness / 2),
		mBallPos.y - (thickness / 2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);

		// Draw paddle
	//SDL_FRect paddle{
	//	static_cast<int>(mPaddlePos.x),
	//	static_cast<int>(mPaddlePos.y - paddelHeight / 2),
	//	thickness,
	//	static_cast<int>(paddelHeight)
	//};
	//SDL_RenderFillRect(mRenderer, &paddle);

	//// Draw ball
	//SDL_FRect ball{
	//	static_cast<int>(mBallPos.x - thickness / 2),
	//	static_cast<int>(mBallPos.y - thickness / 2),
	//	thickness,
	//	thickness
	//};
	//SDL_RenderFillRect(mRenderer, &ball);

	// swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
};