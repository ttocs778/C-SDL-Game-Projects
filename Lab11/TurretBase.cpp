#include "TurretBase.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Game.h"
#include "Math.h"
#include "TurretHead.h"
#include "HealthComponent.h"

TurretBase::TurretBase(Game* game)
: Actor(game)
{
	mTurretBaseCollision = new CollisionComponent(this);
	mTurretBaseMesh = new MeshComponent(this);
	mTurretBaseCollision->SetSize(25.0f, 110.0f, 25.0f);
	SetScale(0.75f);
	SetPosition(Vector3(0.0f, 0.0f, 18.75f));
	mTurretBaseMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/TurretBase.gpmesh"));
	mHead = new TurretHead(game, this);
	mTurretHealth = new HealthComponent(this, 100.0f);
	if (mTurretHealth)
	{
		mTurretHealth->SetOnDeathCallback([this] {
			this->Die();
		});
	}
	game->AddCollider(this);
}

TurretBase::~TurretBase()
{
	mGame->RemoveCollider(this);
}

void TurretBase::Die()
{
	if (mHead)
	{
		mHead->Die();
	}
}