//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <fstream>
#include "Actor.h"
#include "SpriteComponent.h"
#include "SDL2/SDL_image.h"
#include <algorithm>
#include "MoveComponent.h"
#include "Random.h"
#include "Vehicle.h"
#include "Log.h"
#include "Frog.h"

Game::Game()
{
}

bool Game::Initialize()
{
	Random::Init();
	// Initialize SDL with video and audio
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("SDL_Init Error: %s", SDL_GetError());
		return false;
	}

	// Create SDL window
	mWindow = SDL_CreateWindow("Frogger", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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

	//initialize SDL Image
	int imgFlags = IMG_INIT_PNG;
	if (IMG_Init(imgFlags) != imgFlags)
	{
		SDL_Log("IMG_Init Error: %s", IMG_GetError());
		SDL_DestroyRenderer(mRenderer);
		SDL_DestroyWindow(mWindow);
		SDL_Quit();
		return false;
	}

	LoadData();

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

void Game::LoadData()
{
	Actor* starActor = new Actor(this);
	starActor->SetPosition(Vector2(224.0f, 256.0f));
	SpriteComponent* backgroundSprite = new SpriteComponent(starActor, 50);
	backgroundSprite->SetTexture(GetTexture("Assets/Background.png"));
	LoadLevel("Assets/Level.txt");
}

void Game::UnloadData()
{
	while (!mActors.empty())
	{
		delete mActors.back();
	}
	for (auto& pair : mTextures)
	{
		SDL_DestroyTexture(pair.second);
	}
	mTextures.clear();
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	if (mTextures.find(fileName) != mTextures.end())
	{
		return mTextures.find(fileName)->second;
	}
	else
	{
		SDL_Surface* surface = IMG_Load(fileName.c_str());
		if (!surface)
		{
			return nullptr;
		}
		SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
		SDL_FreeSurface(surface);
		if (!texture)
		{
			return nullptr;
		}

		mTextures.emplace(fileName, texture);

		return texture;
	}
}

void Game::AddActor(Actor* actor)
{
	mActors.push_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	auto removeActor = find(mActors.begin(), mActors.end(), actor);
	if (removeActor != mActors.end())
	{
		mActors.erase(removeActor);
	}
}

void Game::AddSprite(SpriteComponent* sprite)
{
	mSprites.push_back(sprite);

	std::sort(mSprites.begin(), mSprites.end(), [](SpriteComponent* a, SpriteComponent* b) {
		return a->GetDrawOrder() < b->GetDrawOrder();
	});
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	auto removeSprite = find(mSprites.begin(), mSprites.end(), sprite);
	if (removeSprite != mSprites.end())
	{
		mSprites.erase(removeSprite);
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

	std::vector<Actor*> actorInput = mActors;

	for (auto* actor : actorInput)
	{
		actor->ProcessInput(state);
	}
}
void Game::AddVehicle(Vehicle* vehicle)
{
	mVehicles.push_back(vehicle);
}

void Game::RemoveVehicle(Vehicle* vehicle)
{
	auto removeVehicle = find(mVehicles.begin(), mVehicles.end(), vehicle);
	if (removeVehicle != mVehicles.end())
	{
		mVehicles.erase(removeVehicle);
	}
}

const std::vector<Vehicle*>& Game::GetVehilce() const
{
	return mVehicles;
}

void Game::AddLog(Log* log)
{
	mLogs.push_back(log);
}

void Game::RemoveLog(Log* log)
{
	auto removeLog = find(mLogs.begin(), mLogs.end(), log);
	if (removeLog != mLogs.end())
	{
		mLogs.erase(removeLog);
	}
}

const std::vector<Log*>& Game::GetLog() const
{
	return mLogs;
}

//Load the game spite based on the text file to set its corresponding position
void Game::LoadLevel(const std::string& fileName)
{
	std::ifstream levelFile(fileName);
	std::string line;
	mRow = 0;
	if (!levelFile.is_open())
	{
		SDL_Log("Failed to open level file: %s", fileName.c_str());
		return;
	}
	while (std::getline(levelFile, line))
	{
		for (int col = 0; col < line.size(); col++)
		{
			char tile = line[col];
			Vector2 position(INITIAL_X + col * SQUARE_SIZE, INITIAL_Y + mRow * SQUARE_SIZE);
			if (tile == 'G')
			{
				// Create a Goal actor at the position
				Actor* goal = new Actor(this);
				goal->SetPosition(position);

				CollisionComponent* goalCollision = new CollisionComponent(goal);
				goalCollision->SetSize(SQUARE_SIZE, SQUARE_SIZE);

				mGoal = goal;
			}
			else if (tile == 'A' || tile == 'B' || tile == 'C' || tile == 'D' || tile == 'T')
			{
				Vehicle* vehicle = new Vehicle(this, tile, mRow);
				vehicle->SetPosition(position);
			}
			else if (tile == 'X' || tile == 'Y' || tile == 'Z')
			{
				Log* log = new Log(this, tile, mRow);
				log->SetPosition(position);
			}
			else if (tile == 'F')
			{
				Frog* frog = new Frog(this);
				mFrog = frog;
				frog->SetPosition(position);
			}
		}
		++mRow;
	}
	levelFile.close();
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

	//Update Actor
	std::vector<Actor*> actorsCopy = mActors;
	std::vector<Actor*> actorDestroy;
	for (Actor* actor : actorsCopy)
	{
		actor->Update(deltaTime);
		if (actor->GetState() == ActorState::Destroy)
		{
			actorDestroy.push_back(actor);
		}
	}
	for (Actor* actor : actorDestroy)
	{
		delete actor;
	}
}

void Game::GenerateOutput()
{
	//create walls
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mRenderer);
	for (auto sprite : mSprites)
	{
		if (sprite->IsVisible())
		{
			sprite->Draw(mRenderer);
		}
	}
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	UnloadData();

	IMG_Quit();

	// Destroy the renderer and window, SDL handles nullptr checks internally
	SDL_DestroyRenderer(mRenderer);

	SDL_DestroyWindow(mWindow);

	// Quit SDL subsystems
	SDL_Quit();
}
