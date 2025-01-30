#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <unordered_map>
#include <string>

class Actor;
class SpriteComponent;
class Asteroid;
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
	void AddAsteroid(Asteroid* asteroid);
	void RemoveAsteroid(Asteroid* asteroid);
	SDL_Texture* GetTexture(const std::string& fileName);
	static constexpr int SCREEN_WIDTH = 1024;
	static constexpr int SCREEN_HEIGHT = 768;
	const std::vector<Asteroid*>& GetAsteroids() const;

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
	std::vector<Asteroid*> mAsteroids;
	std::unordered_map<std::string, SDL_Texture*> mTextures;
	static constexpr float MAX_DELTA_TIME = 0.033f;
};
