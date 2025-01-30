#pragma once
#include "Actor.h"
#include "Game.h"
#include "string.h"

class Door : public Actor
{
public:
	Door(Game* game, const std::string& name);
	~Door();
	bool GetIsOpen() const { return mIsOpen; }
	void SetIsOpen(bool isOpen) { mIsOpen = isOpen; }

private:
	class CollisionComponent* mDoorCollision;
	class MeshComponent* mDoorMesh;
	std::string mName;
	bool mIsOpen = false;
};