//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include <fstream>
#include "Renderer.h"
#include "Random.h"
#include "Player.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "CameraComponent.h"
#include "LevelLoader.h"
#include "Door.h"
#include <SDL2/SDL_ttf.h>

Game::Game()
{
}

bool Game::Initialize()
{
	Random::Init();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mRenderer = new Renderer(this);

	// On Mac, tell SDL that CTRL+Click should generate a Right Click event
	SDL_SetHint(SDL_HINT_MAC_CTRL_CLICK_EMULATE_RIGHT_CLICK, "1");
	// Enable relative mouse mode
	SDL_SetRelativeMouseMode(SDL_TRUE);
	// Clear any saved values
	SDL_GetRelativeMouseState(nullptr, nullptr);

	if (!(mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT)))
	{
		SDL_Log("Unable to initialize Render: %s", SDL_GetError());
		return false;
	}

	mAudio = new AudioSystem(this, 32);

	if (TTF_Init() == -1)
	{
		SDL_Log("Failed to initialize SDL_TTF: %s", TTF_GetError());
		return false;
	}
	LoadData();

	mTicksCount = SDL_GetTicks();

	mInputReplay = new InputReplay(this);

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
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	if (state[SDL_SCANCODE_P])
	{
		mInputReplay->StartPlayback(mCurrentLevel);
	}
	bool f5KeyPressed = state[SDL_SCANCODE_F5];
	if (f5KeyPressed && !mIsF5Pressed)
	{
		mNextLevel = mCurrentLevel;
	}
	mIsF5Pressed = f5KeyPressed;
	int x = 0;
	int y = 0;
	Uint32 mouseButtons = SDL_GetRelativeMouseState(&x, &y);
	Vector2 relativeMouse(x, y);
	mInputReplay->InputPlayback(state, mouseButtons, relativeMouse);
	std::vector<Actor*> copy = mActors;
	for (auto actor : copy)
	{
		actor->ProcessInput(state, mouseButtons, relativeMouse);
	}

	mAudio->ProcessInput(state);
}

void Game::UpdateGame()
{
	// Compute delta time
	Uint32 tickNow = SDL_GetTicks();
	// Wait until 16ms has elapsed since last frame
	while (tickNow - mTicksCount < 16)
	{
		tickNow = SDL_GetTicks();
	}

	// For 3D the games, force delta time to 16ms even if slower
	float deltaTime = 0.016f;
	mTicksCount = tickNow;

	mAudio->Update(deltaTime);
	mInputReplay->Update(deltaTime);

	// Make copy of actor vector
	// (iterate over this in case new actors are created)
	std::vector<Actor*> copy = mActors;
	// Update all actors
	for (auto actor : copy)
	{
		actor->Update(deltaTime);
	}

	// Add any actors to destroy to a temp vector
	std::vector<Actor*> destroyActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == ActorState::Destroy)
		{
			destroyActors.emplace_back(actor);
		}
	}

	// Delete the destroyed actors (which will
	// remove them from mActors)
	for (auto actor : destroyActors)
	{
		delete actor;
	}
	if (!mNextLevel.empty())
	{
		UnloadData();
		mColliders.clear();
		mDoors.clear();
		mInputReplay->StopPlayback();
		mAudio->StopAllSounds();
		mBluePortal = nullptr;
		mOrangePortal = nullptr;
		mCurrentLevel = mNextLevel;
		LevelLoader::Load(this, mCurrentLevel);
		mNextLevel.clear();
	}
}

void Game::GenerateOutput()
{
	mRenderer->Draw();
}

void Game::LoadData()
{
	// Precache all the sounds (do not remove)
	mAudio->CacheAllSounds();
	Matrix4 projectionMatrix = Matrix4::CreatePerspectiveFOV(1.22f, WINDOW_WIDTH, WINDOW_HEIGHT,
															 10.0f, 10000.0f);
	mRenderer->SetProjectionMatrix(projectionMatrix);
	mCurrentLevel = "Assets/Level01.json";
	LevelLoader::Load(this, mCurrentLevel);
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}
}

void Game::Shutdown()
{
	UnloadData();
	delete mAudio;
	mRenderer->Shutdown();
	delete mRenderer;
	delete mInputReplay;
	mInputReplay = nullptr;
	TTF_Quit();
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		mActors.erase(iter);
	}
}

void Game::AddCollider(Actor* collider)
{
	mColliders.emplace_back(collider);
}

void Game::RemoveCollider(Actor* collider)
{
	auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
	if (iter != mColliders.end())
	{
		mColliders.erase(iter);
	}
}

void Game::AddDoor(const std::string& name, Door* door)
{
	mDoors[name] = door;
}

Door* Game::GetDoor(const std::string& name)
{
	auto it = mDoors.find(name);
	return (it != mDoors.end()) ? it->second : nullptr;
}

void Game::RemoveDoor(const std::string& name)
{
	mDoors.erase(name);
}
