#include "Block.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Game.h"
#include "Renderer.h"
#include "CollisionComponent.h"

Block::Block(Game* game)
: Actor(game)
{
	SetScale(64.0f);
	mBlockMesh = new MeshComponent(this);
	mBlockMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/Cube.gpmesh"));
	mBlockCollision = new CollisionComponent(this);
	mBlockCollision->SetSize(1.0f, 1.0f, 1.0f);
	game->AddCollider(this);
}

Block::~Block()
{
	mGame->RemoveActor(this);
}