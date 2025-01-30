#pragma once
#include "Actor.h"

class Player : public Actor
{
public:
	Player(Game* game);
	void OnUpdate(float deltaTime) override;
	bool HasGun() const { return mHasGun; };
	void GiveGun();
	void SetInitialPos(const Vector3& pos) { mInitialPos = pos; }
	const Vector3& GetInitialPos() const { return mInitialPos; }

private:
	class PlayerMove* mPlayerMove;
	class CameraComponent* mCamComponent;
	class CollisionComponent* mPlayerCollision;
	class PlayerMesh* mPlayerMesh = nullptr;
	bool mHasGun = false;
	Vector3 mInitialPos;
	class HealthComponent* mPlayerHealth;
};