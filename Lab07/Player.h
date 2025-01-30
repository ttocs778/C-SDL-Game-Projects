#pragma once
#include "Actor.h"

class Player : public Actor
{
public:
	Player(Game* game);
	class CollisionComponent* GetPlayerColl() { return mPlayerColl; }
	void OnUpdate(float deltaTime) override;
	void TakeDamage();
	int& GetShieldTrack() { return mShieldTrack; }

private:
	class MeshComponent* mMc;
	class CollisionComponent* mPlayerColl;
	class PlayerMove* mPlayerMove;
	class HUD* mHud;
	int mShieldTrack = 3;
	bool mIsInvulnerable = false;
	float mInvulnerabilityTimer = 0.0f;
};