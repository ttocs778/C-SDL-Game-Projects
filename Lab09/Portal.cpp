#include "Portal.h"
#include "Game.h"
#include "PortalMeshComponent.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "Mesh.h"

Portal::Portal(class Game* game, bool isBlue)
: Actor(game)
{
	mPortalMeshComponent = new PortalMeshComponent(this);
	if (isBlue)
	{
		mPortalMeshComponent->SetTextureIndex(0);
	}
	else
	{
		mPortalMeshComponent->SetTextureIndex(1);
	}
	mMesh = new MeshComponent(this, true);
	mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/Portal.gpmesh"));
	if (isBlue)
	{
		mMesh->SetTextureIndex(2);
	}
	else
	{
		mMesh->SetTextureIndex(3);
	}
}
