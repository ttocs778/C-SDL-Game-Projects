#include "EnergyLauncher.h"
#include "CollisionComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "Pellet.h"
#include "Door.h"

EnergyLauncher::EnergyLauncher(Game* game, float cooldown, const std::string& doorName)
: Actor(game)
, mCoolDown(cooldown)
, mDoorName(doorName)
{
	mEnergyLauncherCollision = new CollisionComponent(this);
	mEnergyLauncherMesh = new MeshComponent(this);
	Mesh* energyLauncherMesh = game->GetRenderer()->GetMesh("Assets/Meshes/EnergyLauncher.gpmesh");
	mEnergyLauncherMesh->SetMesh(energyLauncherMesh);
	mEnergyLauncherCollision->SetSize(LAUNCHER_COLL_SIZE, LAUNCHER_COLL_SIZE, LAUNCHER_COLL_SIZE);
	game->AddCollider(this);
}

EnergyLauncher::~EnergyLauncher()
{
	mGame->RemoveCollider(this);
}

void EnergyLauncher::OnUpdate(float deltaTime)
{
	if (!mDoorName.empty())
	{
		Door* door = mGame->GetDoor(mDoorName);
		if (door->GetIsOpen())
		{
			return;
		}
	}
	mElapsedTimes += deltaTime;
	if (mElapsedTimes >= mCoolDown)
	{
		mElapsedTimes = 0.0f;
		mPellet = new Pellet(this->GetGame());
		mGame->GetAudio()->PlaySound("PelletFire.ogg", false, this);
		mPellet->SetPosition(GetPosition() + GetForward() * PELLET_OFFSET);
		mPellet->CalcWorldTransform();
		mPellet->SetVelocity(GetForward() * PELLET_SPEED);
	}
}