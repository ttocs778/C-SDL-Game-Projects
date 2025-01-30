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
#include "SideBlock.h"
#include "Block.h"
#include "MeshComponent.h"

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

	LoadNewBlocksIfNeeded();
	SetExplosionSoundPlayed(false);
}

void Game::GenerateOutput()
{
	mRenderer->Draw();
}

void Game::LoadData()
{
	// Precache all the sounds (do not remove)
	mAudio->CacheAllSounds();
	mPlayer = new Player(this);
	Matrix4 projectionMatrix = Matrix4::CreatePerspectiveFOV(1.22f, WINDOW_WIDTH, WINDOW_HEIGHT,
															 10.0f, 10000.0f);
	mRenderer->SetProjectionMatrix(projectionMatrix);
	Matrix4 cameraMatrix = Matrix4::CreateLookAt(mCameraPos, mTargetPos, Vector3::UnitZ);
	mRenderer->SetViewMatrix(cameraMatrix);
	for (int i = 1; i <= 20; ++i)
	{
		LoadBlocks("Assets/Blocks/" + std::to_string(i) + ".txt");
		mBlockXPos += 1000.0f;
	}
	if (mAudio)
	{
		mAudio->PlaySound("Music.ogg", true);
	}
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

void Game::AddBlock(Block* block)
{
	mBlocks.push_back(block);
}

void Game::RemoveBlock(Block* block)
{
	auto removeBlock = find(mBlocks.begin(), mBlocks.end(), block);
	if (removeBlock != mBlocks.end())
	{
		mBlocks.erase(removeBlock);
	}
}

void Game::LoadNewBlocksIfNeeded()
{
	Vector3 playerPos = mPlayer->GetPosition();
	while (mBlockXPos < playerPos.x + 4000.0f)
	{
		int nextFileIndex = static_cast<int>(mBlockXPos / 1000.0f) + 1;
		std::string fileName;
		if (nextFileIndex <= 20)
		{
			fileName = "Assets/Blocks/" + std::to_string(nextFileIndex) + ".txt";
		}
		else
		{
			int randomIndex = Random::GetIntRange(1, 20);
			fileName = "Assets/Blocks/" + std::to_string(randomIndex) + ".txt";
		}
		LoadBlocks(fileName);
		mBlockXPos += 1000.0f;
	}
}

void Game::LoadBlocks(const std::string& fileName)
{
	std::ifstream levelFile(fileName);
	std::string line;
	if (!levelFile.is_open())
	{
		SDL_Log("Failed to open level file: %s", fileName.c_str());
		return;
	}
	mRow = 0;
	while (std::getline(levelFile, line))
	{
		for (int col = 0; col < line.size(); col++)
		{
			char tile = line[col];
			Vector3 position(mBlockXPos, -TOPLEFT_Y + (col * INC_NUM),
							 TOPLEFT_Z - (mRow * INC_NUM));

			if (tile == '.')
			{
				continue;
			}
			Block::BlockType type = (tile == 'B') ? Block::Exploding : Block::Regular;
			Block* block = new Block(this, type);
			block->SetPosition(position);
		}
		++mRow;
	}
}

void Game::PlayShipLoop()
{
	if (!mIsShipLoopPlaying)
	{
		mShipLoopHandle = mAudio->PlaySound("ShipLoop.ogg", true);
		mIsShipLoopPlaying = true;
	}
}

void Game::StopShipLoop()
{
	if (mIsShipLoopPlaying)
	{
		mAudio->StopSound(mShipLoopHandle);
		mIsShipLoopPlaying = false;
	}
}

void Game::PlayDamageAlert()
{
	if (!mIsDamageAlertPlaying)
	{
		mDamageAlertHandle = mAudio->PlaySound("DamageAlert.ogg", true); // Loop damage alert
		mIsDamageAlertPlaying = true;
	}
}

void Game::StopDamageAlert()
{
	if (mIsDamageAlertPlaying)
	{
		mAudio->StopSound(mDamageAlertHandle);
		mIsDamageAlertPlaying = false;
	}
}
