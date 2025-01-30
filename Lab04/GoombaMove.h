#pragma once
#include "MoveComponent.h"
#include "Math.h"

class Goomba;
class GoombaMove : public MoveComponent
{
public:
	GoombaMove(Actor* owner);
	void Update(float deltaTime) override;

private:
	float mYSpeed = 0.0f;
	float mGravity = 2000.0f;
};
