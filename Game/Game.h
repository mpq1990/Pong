#pragma once

struct SDL_Window;
struct SDL_Renderer;

struct Vector2 {
	float x, y;
};

class Game
{
public:
	Game();

	bool Initialize();

	void RunLoop();

	void Shutdown();
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	

	//Window created by SDL
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	Vector2 mPaddlePos;
	Vector2 mBallPos;

	bool mIsRunning;
};

