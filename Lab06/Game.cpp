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
#include "Player.h"
#include "TiledBGComponent.h"
#include "Bush.h"
#include "Soldier.h"
#include "PathNode.h"
#include "EnemyComponent.h"
#include "AudioSystem.h"

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
	mWindow = SDL_CreateWindow("Zelda", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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
	//load sound
	mAudio = new AudioSystem();
	if (!mAudio)
	{
		delete mAudio;
		mAudio = nullptr;
	}
	mMusicStartHandle = mAudio->PlaySound("MusicStart.ogg", false);

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
	if (mAudio)
	{
		mAudio->CacheAllSounds();
	}
	Actor* bgActor = new Actor(this);
	TiledBGComponent* tiledBG = new TiledBGComponent(bgActor, 150);
	tiledBG->LoadTileCSV("Assets/Map/Tiles.csv", 32, 32);
	tiledBG->SetTexture(GetTexture("Assets/Map/Tiles.png"));
	LoadObjCSV("Assets/Map/Objects.csv");
}

void Game::LoadObjCSV(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("Failed to load CSV: %s", fileName.c_str());
		return;
	}
	std::string line;
	std::getline(file, line);
	while (std::getline(file, line))
	{
		std::vector<std::string> strings = CSVHelper::Split(line);
		if (strings.size() > 4)
		{
			int x = std::stoi(strings[1]);
			int y = std::stoi(strings[2]);
			int width = std::stoi(strings[3]);
			int height = std::stoi(strings[4]);
			x += width / 2;
			y += height / 2;
			std::string objectType = strings[0];
			if (objectType == "Player")
			{
				mPlayer = new Player(this);
				mPlayer->SetPosition(Vector2(x, y));
			}
			if (objectType == "Collider")
			{
				Collider* collider = new Collider(this, static_cast<float>(width),
												  static_cast<float>(height));
				collider->SetPosition(Vector2(x, y));
				mColliders.push_back(collider);
			}

			if (objectType == "Bush")
			{
				Bush* bush = new Bush(this);
				bush->SetPosition(Vector2(x, y));
			}
			if (objectType == "Soldier")
			{
				PathNode* start = mPathFinder->GetPathNode(std::stoi(strings[5]),
														   std::stoi(strings[6]));
				PathNode* end = mPathFinder->GetPathNode(std::stoi(strings[7]),
														 std::stoi(strings[8]));
				Soldier* soldier = new Soldier(this, start, end);
				soldier->SetPosition(Vector2(x, y));
			}
		}
	}
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
	if (mAudio)
	{
		if (!mIsLoopPlaying && mAudio->GetSoundState(mMusicStartHandle) == SoundState::Stopped)
		{
			mAudio->PlaySound("MusicLoop.ogg", true);
			mIsLoopPlaying = true;
		}
		mAudio->Update(deltaTime);
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
	if (mAudio)
	{
		delete mAudio;
		mAudio = nullptr;
	}

	UnloadData();

	IMG_Quit();

	// Destroy the renderer and window, SDL handles nullptr checks internally
	SDL_DestroyRenderer(mRenderer);

	SDL_DestroyWindow(mWindow);

	// Quit SDL subsystems
	SDL_Quit();
}
