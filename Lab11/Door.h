#pragma once
#include "Actor.h"
#include "Game.h"
#include "string.h"

class Door : public Actor
{
public:
	Door(Game* game, const std::string& name);
	~Door();
	void OnUpdate(float deltaTime) override;
	bool GetIsOpen() const { return mIsOpen; }
	void SetIsOpen(bool isOpen)
	{
		if (isOpen != mIsOpen)
		{
			mIsOpen = isOpen;
		}
	}

private:
	class CollisionComponent* mDoorCollision;
	class MeshComponent* mDoorMesh;
	std::string mName;
	bool mIsOpen = false;
	const float OPEN_TIME_MAX = 1.0f;
	float mOpenTimer = 0.0f;
	Actor* mLeftDoor;
	Actor* mRightDoor;
};