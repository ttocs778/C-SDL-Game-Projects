#pragma once
#include "Actor.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include <string>

class Prop : public Actor
{
public:
	Prop(Game* game, bool usesAlpha, bool hasCollision, const std::string& meshPath);
	~Prop();

private:
	class MeshComponent* mMeshComponent;
	class CollisionComponent* mCollisionComponent;
};