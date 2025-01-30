#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>

class Frog;
class Actor;
class SpriteComponent;
class Vehicle;
class Log;
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
	void AddVehicle(Vehicle* vehicle);
	void RemoveVehicle(Vehicle* vehicle);
	void AddLog(Log* log);
	void RemoveLog(Log* log);
	const std::vector<Vehicle*>& GetVehilce() const;
	const std::vector<Log*>& GetLog() const;
	Actor* GetGoal() const { return mGoal; }
	void LoadLevel(const std::string& fileName);
	SDL_Texture* GetTexture(const std::string& fileName);
	static constexpr int SCREEN_WIDTH = 448;
	static constexpr int SCREEN_HEIGHT = 512;
	Frog* GetFrog() const { return mFrog; }

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
	std::unordered_map<std::string, SDL_Texture*> mTextures;
	std::vector<Vehicle*> mVehicles;
	std::vector<Log*> mLogs;
	static constexpr float MAX_DELTA_TIME = 0.033f;
	const float SQUARE_SIZE = 32.0f;
	const int INITIAL_X = 32;
	const int INITIAL_Y = 80;
	int mRow = 0;
	Frog* mFrog = nullptr;
	Actor* mGoal = nullptr;
};
