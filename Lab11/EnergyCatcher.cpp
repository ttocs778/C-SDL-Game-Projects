#include "EnergyCatcher.h"
#include "CollisionComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "Door.h"

EnergyCatcher::EnergyCatcher(Game* game, const std::string& doorName)
: Actor(game)
, mDoorName(doorName)
{
	mEnergyCatcherCollision = new CollisionComponent(this);
	mEnergyCatcherMesh = new MeshComponent(this);
	Mesh* energyCatcherMesh = game->GetRenderer()->GetMesh("Assets/Meshes/EnergyLauncher.gpmesh");
	mEnergyCatcherMesh->SetMesh(energyCatcherMesh);
	mEnergyCatcherCollision->SetSize(CATCHER_COLL_SIZE, CATCHER_COLL_SIZE, CATCHER_COLL_SIZE);
	game->AddCollider(this);
}

EnergyCatcher::~EnergyCatcher()
{
	mGame->RemoveCollider(this);
}

void EnergyCatcher::SetIsActivated(bool isActivated)
{
	if (isActivated && !mIsActivated)
	{
		mIsActivated = true;
		if (!mDoorName.empty())
		{
			Door* door = mGame->GetDoor(mDoorName);
			if (door)
			{
				mGame->RemoveCollider(door);
				door->SetIsOpen(true);
			}
		}
	}
}