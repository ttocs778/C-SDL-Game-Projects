#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include "Math.h"
#include "SDL2/SDL_mixer.h"
#include "Collider.h"
#include "PathFinder.h"
#include "AudioSystem.h"

class Player;
class Actor;
class SpriteComponent;
class EnemyComponent;
class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();
	void AddActor(Actor* actor);
	void RemoveActor(Actor* actor);
	void AddSprite(SpriteComponent* sprite);
	void RemoveSprite(SpriteComponent* sprite);
	SDL_Texture* GetTexture(const std::string& fileName);
	Player* GetPlayer() const { return mPlayer; }
	Vector2& GetCameraPos() { return mCameraPos; }
	const std::vector<Collider*>& GetColliders() const { return mColliders; }
	std::vector<EnemyComponent*>& GetEnemies() { return mEnemies; }
	static constexpr int SCREEN_WIDTH = 512;
	static constexpr int SCREEN_HEIGHT = 448;
	void LoadObjCSV(const std::string& fileName);
	PathFinder* GetPathFinder() const { return mPathFinder; }
	AudioSystem* GetAudio() const { return mAudio; }

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();
	bool mIsRunning = true;
	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;
	Uint32 mPreviousTicks = 0;
	std::vector<Actor*> mActors;
	std::vector<SpriteComponent*> mSprites;
	std::vector<Collider*> mColliders;
	std::vector<EnemyComponent*> mEnemies;
	std::unordered_map<std::string, SDL_Texture*> mTextures;
	static constexpr float MAX_DELTA_TIME = 0.033f;
	Player* mPlayer = nullptr;
	Vector2 mCameraPos{0.0f, 0.0f};
	PathFinder* mPathFinder = new PathFinder(this);
	AudioSystem* mAudio = nullptr;
	SoundHandle mMusicStartHandle;
	bool mIsLoopPlaying = false;
};
