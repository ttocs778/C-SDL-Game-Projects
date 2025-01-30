#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"

class AnimatedSprite;
class GoombaMove;
class Goomba : public Actor
{
public:
	Goomba(Game* game);
	~Goomba();
	bool IsStomped() const { return mStomped; }
	float GetDestroyTimer() const { return mDestroyTimer; }
	void SetDestroyTimer(float destroyTimer) { mDestroyTimer = destroyTimer; }
	void SetStomped() { mStomped = true; }
	void GetStomped();
	AnimatedSprite* GetAnimation() { return mGoombaAnimation; }

private:
	SpriteComponent* mGoombaSprite;
	CollisionComponent* mGoombaCollision;
	const float GOOMBA_SIZE = 32.0f;
	GoombaMove* mGoombaMove;
	bool mStomped = false;
	float mDestroyTimer = 0.0f;
	AnimatedSprite* mGoombaAnimation;
};