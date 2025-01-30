#include "Mesh.h"
#include "Game.h"
#include "PortalGun.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Player.h"

PortalGun::PortalGun(Game* game)
: Actor(game)
{
	mCollisionGun = new CollisionComponent(this);
	mCollisionGun->SetSize(8.0f, 8.0f, 8.0f);
	mMeshGun = new MeshComponent(this);
	mMeshGun->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/PortalGun.gpmesh"));
}

void PortalGun::OnUpdate(float deltaTime)
{
	float rotationAmount = ROTATION_SPEED * deltaTime;
	float newRotation = GetRotation() + rotationAmount;
	SetRotation(newRotation);
	CollisionComponent* collPlayer = mGame->GetPlayer()->GetComponent<CollisionComponent>();
	if (collPlayer->Intersect(mCollisionGun))
	{
		SetState(ActorState::Destroy);
		mGame->GetPlayer()->GiveGun();
	}
}