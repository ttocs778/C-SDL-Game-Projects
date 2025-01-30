#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include "AudioSystem.h"
#include "Player.h"
#include "PlayerMove.h"
#include "CameraComponent.h"
#include "Portal.h"
#include "InputReplay.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define emscripten_cancel_main_loop()
#endif

class Door;
class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void EmRunIteration()
	{
		if (!mIsRunning)
		{
			emscripten_cancel_main_loop();
		}
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	void AddCollider(class Actor* collider);
	void RemoveCollider(class Actor* collider);

	AudioSystem* GetAudio() { return mAudio; }

	const std::vector<class Actor*>& GetCollider() const { return mColliders; }
	const std::vector<class Actor*>& GetActor() const { return mActors; }
	class Renderer* GetRenderer() { return mRenderer; }
	class Player* GetPlayer() { return mPlayer; }
	void SetPlayer(Player* player) { mPlayer = player; }

	const float WINDOW_WIDTH = 1024.0f;
	const float WINDOW_HEIGHT = 768.0f;
	SoundHandle GetMusicHandle() const { return mMusicHandle; }
	void SetMusicHandle(SoundHandle handle) { mMusicHandle = handle; }

	class Portal* GetBluePortal() { return mBluePortal; }
	class Portal* GetOrangePortal() { return mOrangePortal; }
	void SetBluePortal(class Portal* bluePortal) { mBluePortal = bluePortal; }
	void SetOrangePortal(class Portal* orangePortal) { mOrangePortal = orangePortal; }
	void AddDoor(const std::string& name, Door* door);
	Door* GetDoor(const std::string& name);
	void RemoveDoor(const std::string& name);
	void SetNextLevel(std::string nextLevel) { mNextLevel = nextLevel; }
	std::string GetCurrentLevel() const { return mCurrentLevel; }
	void ReloadLevel();

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	// All the actors in the game
	std::vector<class Actor*> mActors;
	std::vector<class Actor*> mColliders;
	std::unordered_map<std::string, Door*> mDoors;

	class Renderer* mRenderer = nullptr;
	AudioSystem* mAudio = nullptr;

	Uint32 mTicksCount = 0;
	bool mIsRunning = true;
	Player* mPlayer = nullptr;
	float mStartTimer = 8.5f;
	SoundHandle mMusicHandle;
	Portal* mBluePortal = nullptr;
	Portal* mOrangePortal = nullptr;
	std::string mCurrentLevel = " ";
	std::string mNextLevel;
	InputReplay* mInputReplay = nullptr;
	bool mIsF5Pressed = false;
};
