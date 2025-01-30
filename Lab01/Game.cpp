//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"

Game::Game()
{
}

bool Game::Initialize()
{
	// Initialize SDL with video and audio
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("SDL_Init Error: %s", SDL_GetError());
		return false;
	}

	// Create SDL window
	mWindow = SDL_CreateWindow("Pong", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (!mWindow)
	{
		SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
		SDL_Quit();
		return false;
	}

	// Create SDL renderer
	mRenderer = SDL_CreateRenderer(mWindow, -1,
								   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer)
	{
		SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
		SDL_DestroyWindow(mWindow);
		SDL_Quit();
		return false;
	}

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	// create a event to store the user input
	SDL_Event event;
	/* use SDL_PollEvent to poll user input stored in the event pointer,
	and we use & since this function requires a pointer as a parameter */
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	//check the esc press
	const Uint8* state = SDL_GetKeyboardState(nullptr);
	if (state[SDL_SCANCODE_ESCAPE] == 1)
	{
		mIsRunning = false;
	}

	//check the moving press
	if (state[SDL_SCANCODE_W] == 1)
	{
		mPaddleDirection = -1;
	}
	else if (state[SDL_SCANCODE_S] == 1)
	{
		mPaddleDirection = 1;
	}
	else
	{
		mPaddleDirection = 0;
	}
}

void Game::UpdateGame()
{
	//check the delta time
	Uint32 currentTicks = SDL_GetTicks();
	mPreviousTicks = currentTicks;

	//set the frame limit to 60, which should limit its diff in 16ms
	while ((currentTicks - mPreviousTicks) < 16)
	{
		//update the current ticks, loop until the diff reaches 16
		currentTicks = SDL_GetTicks();
	}

	//diff is 16 so deltatime can be computed
	float deltaTime = (currentTicks - mPreviousTicks) / 1000.0f;

	//set deltatime cap
	if (deltaTime > MAX_DELTA_TIME)
	{
		deltaTime = MAX_DELTA_TIME;
	}

	//moving the paddle
	mPaddlePos.y += static_cast<int>(mPaddleDirection * deltaTime * PADDLE_SPEED);
	if (mPaddlePos.y - (PADDLE_HEIGHT / 2) < WALL_THICKNESS)
	{
		mPaddlePos.y = WALL_THICKNESS + (PADDLE_HEIGHT / 2);
	}
	else if (mPaddlePos.y + (PADDLE_HEIGHT / 2) > SCREEN_HEIGHT - WALL_THICKNESS)
	{
		mPaddlePos.y = SCREEN_HEIGHT - WALL_THICKNESS - (PADDLE_HEIGHT / 2);
	}

	//moving the ball
	mBallPos.x += static_cast<int>(deltaTime * mBallVelocity.x);
	mBallPos.y += static_cast<int>(deltaTime * mBallVelocity.y);

	//bounce the wall
	//Check for collision with the top wall
	if (mBallPos.y - (BALL_HEIGHT / 2) <= WALL_THICKNESS)
	{
		// Ball hit the top wall
		mBallPos.y = WALL_THICKNESS + (BALL_HEIGHT / 2);
		mBallVelocity.y = -mBallVelocity.y; // Reverse the y-velocity
	}

	//Check for collision with the bottom wall
	if (mBallPos.y + (BALL_HEIGHT / 2) >= SCREEN_HEIGHT - WALL_THICKNESS)
	{
		// Ball hit the bottom wall
		mBallPos.y = SCREEN_HEIGHT - WALL_THICKNESS - (BALL_HEIGHT / 2);
		mBallVelocity.y = -mBallVelocity.y; // Reverse the y-velocity
	}

	//Check for collision with the right wall
	if (mBallPos.x + (BALL_WIDTH / 2) >= SCREEN_WIDTH - WALL_THICKNESS - (BALL_WIDTH / 2))
	{
		// Ball hit the right wall
		mBallVelocity.x *= -1;
		mBallPos.x = SCREEN_WIDTH - WALL_THICKNESS - (BALL_WIDTH / 2);
	}
	mPreviousTicks = currentTicks;

	//Initialization for projection
	int ballLeft = mBallPos.x - BALL_WIDTH / 2;
	int ballRight = mBallPos.x + BALL_WIDTH / 2;
	int ballTop = mBallPos.y - BALL_WIDTH / 2;
	int ballBottom = mBallPos.y + BALL_WIDTH / 2;
	int paddleLeft = mPaddlePos.x - PADDLE_WIDTH / 2;
	int paddleRight = mPaddlePos.x + PADDLE_WIDTH / 2;
	int paddleTop = mPaddlePos.y - PADDLE_HEIGHT / 2;
	int paddleBottom = mPaddlePos.y + PADDLE_HEIGHT / 2;

	//Check for collision with the paddle
	if (ballLeft <= paddleRight && ballRight >= paddleLeft && ballBottom >= paddleTop &&
		ballTop <= paddleBottom &&
		mBallPos.x - (BALL_WIDTH / 2) <= mPaddlePos.x + (PADDLE_WIDTH / 2))
	{
		mBallVelocity.x *= -1; // Reverse the x velocity
		mBallPos.x = mPaddlePos.x + (PADDLE_WIDTH / 2) + (BALL_WIDTH / 2);
	}
	else if (mBallPos.x - (BALL_WIDTH / 2) < 0)
	{
		mIsRunning = false;
	}
}

void Game::GenerateOutput()
{
	//create walls
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);
	SDL_RenderClear(mRenderer);
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	SDL_Rect topWall;
	topWall.x = 0;
	topWall.y = 0;
	topWall.w = SCREEN_WIDTH;
	topWall.h = WALL_THICKNESS;
	SDL_RenderFillRect(mRenderer, &topWall);

	SDL_Rect rightWall;
	rightWall.x = SCREEN_WIDTH - WALL_THICKNESS;
	rightWall.y = 0;
	rightWall.w = WALL_THICKNESS;
	rightWall.h = SCREEN_HEIGHT;
	SDL_RenderFillRect(mRenderer, &rightWall);

	SDL_Rect bottomWall;
	bottomWall.x = 0;
	bottomWall.y = SCREEN_HEIGHT - WALL_THICKNESS;
	bottomWall.w = SCREEN_WIDTH - WALL_THICKNESS;
	bottomWall.h = WALL_THICKNESS;
	SDL_RenderFillRect(mRenderer, &bottomWall);

	//creat paddle
	SDL_Rect paddle;
	paddle.h = PADDLE_HEIGHT;
	paddle.w = PADDLE_WIDTH;
	paddle.x = mPaddlePos.x - (PADDLE_WIDTH) / 2;
	paddle.y = mPaddlePos.y - (PADDLE_HEIGHT) / 2;
	SDL_RenderFillRect(mRenderer, &paddle);

	//create ball
	SDL_Rect ball;
	ball.h = BALL_HEIGHT;
	ball.w = BALL_WIDTH;
	ball.x = mBallPos.x - (BALL_WIDTH) / 2;
	ball.y = mBallPos.y - (BALL_HEIGHT) / 2;
	SDL_RenderFillRect(mRenderer, &ball);
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	// Destroy the renderer and window, SDL handles nullptr checks internally
	SDL_DestroyRenderer(mRenderer);
	mRenderer = nullptr;

	SDL_DestroyWindow(mWindow);
	mWindow = nullptr;

	// Quit SDL subsystems
	SDL_Quit();
}
