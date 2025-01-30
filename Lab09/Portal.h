#pragma once
#include "Actor.h"

class Portal : public Actor
{
public:
	Portal(class Game* game, bool isBlue);

private:
	class PortalMeshComponent* mPortalMeshComponent;
	class MeshComponent* mMesh;
};
