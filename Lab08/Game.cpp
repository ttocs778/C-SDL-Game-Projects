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
#include "HeightMap.h"
#include "Enemy.h"
#include "CameraComponent.h"

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
	if (!(mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT)))
	{
		SDL_Log("Unable to initialize Render: %s", SDL_GetError());
		return false;
	}

	mAudio = new AudioSystem();

	LoadData();

	mTicksCount = SDL_GetTicks();

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

	std::vector<Actor*> copy = mActors;
	for (auto actor : copy)
	{
		actor->ProcessInput(state);
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

	if (mStartTimer > 0.0f)
	{
		mStartTimer -= deltaTime;
		if (mStartTimer <= 0.0f)
		{
			mMusicHandle = mAudio->PlaySound("Music.ogg", true);
			if (!mMusicHandle.IsValid())
			{
				SDL_Log("Failed to play music: Assets/Audio/Music.ogg");
			}
			mPlayer->SetState(ActorState::Active);
			mEnemy->SetState(ActorState::Active);
		}
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
	mHeightMap = new HeightMap("Assets/HeightMap/HeightMap.csv");
	mPlayer = new Player(this);
	mPlayer->SetState(ActorState::Paused);
	mEnemy = new Enemy(this);
	mEnemy->SetState(ActorState::Paused);
	Matrix4 projectionMatrix = Matrix4::CreatePerspectiveFOV(1.22f, WINDOW_WIDTH, WINDOW_HEIGHT,
															 10.0f, 10000.0f);
	mRenderer->SetProjectionMatrix(projectionMatrix);
	Actor* actor = new Actor(this);
	actor->SetRotation(Math::Pi);
	MeshComponent* actorMc = new MeshComponent(actor);
	actorMc->SetMesh(mRenderer->GetMesh("Assets/Track.gpmesh"));
	mAudio->PlaySound("RaceStart.wav");
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
