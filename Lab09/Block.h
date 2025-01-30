#pragma once
#include "Actor.h"

class Block : public Actor
{
public:
	Block(Game* game);
	~Block();

private:
	class MeshComponent* mBlockMesh;
	class CollisionComponent* mBlockCollision;
};