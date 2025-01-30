#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include "Math.h"
#include "SDL2/SDL_mixer.h"

class Player;
class Actor;
class SpriteComponent;
class Block;
class Goomba;
class Spawner;
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
	Actor* GetGoal() const { return mGoal; }
	void LoadLevel(const std::string& fileName);
	SDL_Texture* GetTexture(const std::string& fileName);
	void AddBlock(Block* block);
	void RemoveBlock(Block* block);
	const std::vector<Block*>& GetBlock() const;
	void AddGoomba(Goomba* goomba);
	void RemoveGoomba(Goomba* goomba);
	const std::vector<Goomba*>& GetGoomba() const;
	class Player* GetPlayer() const { return mPlayer; }
	Vector2& GetCameraPos() { return mCameraPos; }
	static constexpr int SCREEN_WIDTH = 600;
	static constexpr int SCREEN_HEIGHT = 448;
	Mix_Chunk* GetSound(const std::string& filename);
	int GetLoopSound() const { return mSoundLoopNum; }

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
	std::vector<Block*> mBlocks;
	std::vector<Goomba*> mGoombas;
	std::unordered_map<std::string, SDL_Texture*> mTextures;
	static constexpr float MAX_DELTA_TIME = 0.033f;
	const int SQUARE_SIZE = 32;
	const int INITIAL_X = 16;
	const int INITIAL_Y = 16;
	const float BACKGROUND_X = 3392.0f;
	const float BACKGROUND_Y = 224.0f;
	int mRow = 0;
	Actor* mGoal = nullptr;
	Player* mPlayer = nullptr;
	Vector2 mCameraPos{0.0f, 0.0f};
	std::unordered_map<std::string, Mix_Chunk*> mSounds;
	int mSoundLoopNum = 0;
};
