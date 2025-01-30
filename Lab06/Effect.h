#pragma once
#include "Actor.h"
#include "Game.h"
#include "Math.h"

class Effect : public Actor
{
public:
	Effect(Game* game, const Vector2& effectPos, const std::string& animName,
		   const std::string& soundName);
	void OnUpdate(float deltaTime) override;

private:
	float mLifeTime{0.0};
};