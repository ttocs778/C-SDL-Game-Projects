#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include <unordered_map>
#include "CollisionComponent.h"
#include "Game.h"
#include "Vehicle.h"

class Frog : public Actor
{
public:
	Frog(Game* game);
	void OnProcessInput(const Uint8* keyState) override;
	void OnUpdate(float deltaTime) override;

private:
	SpriteComponent* mFrogSprite;
	std::unordered_map<SDL_Scancode, bool> mKeyLastFrame;
	const float MOVE_DISTANCE = 32.0f;
	const float MIN_X = 0.0f;
	const float MAX_X = Game::SCREEN_WIDTH - 32.0f;
	const float MIN_Y = 0.0f;
	const float MAX_Y = Game::SCREEN_HEIGHT - 16.0f;
	CollisionComponent* mFrogCollision;
	const float WATER_MIN_Y = 90.0f;
	const float WATER_MAX_Y = 255.0f;
	const float FROG_START_X = 224.0f;
	const float FROG_START_Y = 464.0f;
};