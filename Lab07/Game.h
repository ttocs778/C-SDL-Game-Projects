#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include "AudioSystem.h"
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

	AudioSystem* GetAudio() { return mAudio; }

	class Renderer* GetRenderer() { return mRenderer; }
	class Player* GetPlayer() { return mPlayer; }

	const std::vector<class Block*>& GetBlocks() const { return mBlocks; }
	void AddBlock(class Block* block);
	void RemoveBlock(class Block* block);
	const float WINDOW_WIDTH = 1024.0f;
	const float WINDOW_HEIGHT = 768.0f;
	void LoadBlocks(const std::string& fileName);
	void LoadNewBlocksIfNeeded();
	void PlayShipLoop();
	void StopShipLoop();
	void PlayDamageAlert();
	void StopDamageAlert();
	void SetExplosionSoundPlayed(bool played) { mExplosionSoundPlayed = played; }
	bool HasExplosionSoundPlayed() const { return mExplosionSoundPlayed; }

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	// All the actors in the game
	std::vector<class Actor*> mActors;

	class Renderer* mRenderer = nullptr;
	AudioSystem* mAudio = nullptr;

	Uint32 mTicksCount = 0;
	bool mIsRunning = true;
	class Player* mPlayer = nullptr;
	Vector3 mCameraPos{-300, 0, 0};
	Vector3 mTargetPos{20, 0, 0};
	std::vector<class Block*> mBlocks;
	float mBlockXPos = 1000.0f;
	int mRow = 0;
	bool mIsShipLoopPlaying = false;
	bool mIsDamageAlertPlaying = false;
	SoundHandle mShipLoopHandle;
	SoundHandle mDamageAlertHandle;
	const float TOPLEFT_Y = 237.5;
	const float TOPLEFT_Z = 237.5;
	const float INC_NUM = 25.0f;
	bool mExplosionSoundPlayed = false;
};
