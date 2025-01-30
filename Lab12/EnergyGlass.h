#pragma once
#include "Actor.h"
#include "Game.h"
#include "string.h"

class EnergyGlass : public Actor
{
public:
	EnergyGlass(Game* game);
	~EnergyGlass();

private:
	class CollisionComponent* mEnergyGlassCollision;
	class MeshComponent* mEnergyGlassMesh;
};