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
	/*
	Vector3 GetPlayerPosition() const { return mPlayer->GetPosition(); }
	Vector3 GetPlayerAcceleration() const { return mPlayer->GetComponent<PlayerMove>()->GetAcc(); }
	Vector3 GetPlayerVelocity() const { return mPlayer->GetComponent<PlayerMove>()->GetVel(); }
	float GetPlayerYaw() const { return mPlayer->GetRotation(); }
	float GetPlayerPitch() const
	{
		return mPlayer->GetComponent<CameraComponent>()->GetPitchAngle();
	}
	*/

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	// All the actors in the game
	std::vector<class Actor*> mActors;

	std::vector<class Actor*> mColliders;

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
	InputReplay* mInputReplay = nullptr;
};
