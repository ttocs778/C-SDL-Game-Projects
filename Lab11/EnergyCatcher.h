#pragma once
#include "Actor.h"
#include "Game.h"
#include "string.h"

class EnergyCatcher : public Actor
{
public:
	EnergyCatcher(Game* game, const std::string& doorName);
	~EnergyCatcher();
	bool GetIsActivated() const { return mIsActivated; }
	void SetIsActivated(bool isActivated);

private:
	class CollisionComponent* mEnergyCatcherCollision;
	class MeshComponent* mEnergyCatcherMesh;
	bool mIsActivated = false;
	std::string mDoorName;
	const float CATCHER_COLL_SIZE = 50.0f;
};