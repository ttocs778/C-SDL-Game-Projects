#pragma once
#include "Actor.h"

class SideBlock : public Actor
{
public:
	SideBlock(Game* game, size_t textureIndex);
	void OnUpdate(float deltaTime) override;

private:
	class MeshComponent* mMc;
};