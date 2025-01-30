#include "Prop.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "Renderer.h"

Prop::Prop(Game* game, bool usesAlpha, bool hasCollision, const std::string& meshPath)
: Actor(game)
{
	mMeshComponent = new MeshComponent(this, usesAlpha);
	Mesh* mesh = game->GetRenderer()->GetMesh(meshPath);
	mMeshComponent->SetMesh(mesh);
	if (hasCollision)
	{
		mCollisionComponent = new CollisionComponent(this);

		mCollisionComponent->SetSize(mesh->GetWidth(), mesh->GetHeight(), mesh->GetDepth());

		game->AddCollider(this);
	}
	else
	{
		mCollisionComponent = nullptr;
	}
}

Prop::~Prop()
{
	if (mCollisionComponent)
	{
		mGame->RemoveCollider(this);
	}
}
