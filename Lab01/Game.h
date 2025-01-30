#pragma once
#include <SDL2/SDL.h>

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

	bool mIsRunning = true;
	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;
	Uint32 mPreviousTicks = 0;
	int mPaddleDirection = 0;

	static constexpr int SCREEN_WIDTH = 1024;
	static constexpr int SCREEN_HEIGHT = 768;
	static constexpr int WALL_THICKNESS = 12;
	static constexpr int PADDLE_WIDTH = 12;
	static constexpr int PADDLE_HEIGHT = 90;
	static constexpr int BALL_WIDTH = 8;
	static constexpr int BALL_HEIGHT = 8;
	static constexpr float PADDLE_SPEED = 600.0f;
	static constexpr float MAX_DELTA_TIME = 0.033f;

	SDL_Point mPaddlePos = {14, 384};
	SDL_Point mBallPos = {512, 384};
	SDL_Point mBallVelocity = {400, 400};
};
