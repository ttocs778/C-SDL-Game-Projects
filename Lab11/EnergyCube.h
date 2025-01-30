#pragma once
#include "Actor.h"
#include "Game.h"
#include "string.h"

class EnergyCube : public Actor
{
public:
	EnergyCube(Game* game);
	~EnergyCube();

private:
	class CollisionComponent* mEnergyCubeCollision;
	class MeshComponent* mEnergyCubeMesh;
};