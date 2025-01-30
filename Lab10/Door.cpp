#include "Door.h"
#include "CollisionComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "MeshComponent.h"

Door::Door(Game* game, const std::string& name)
: Actor(game)
, mName(name)
{
	mDoorCollision = new CollisionComponent(this);
	mDoorMesh = new MeshComponent(this);
	Mesh* doorMesh = game->GetRenderer()->GetMesh("Assets/Meshes/DoorFrame.gpmesh");
	mDoorMesh->SetMesh(doorMesh);
	mDoorCollision->SetSize(doorMesh->GetWidth(), doorMesh->GetHeight(), doorMesh->GetDepth());
	game->AddCollider(this);
	game->AddDoor(name, this);
}

Door::~Door()
{
	mGame->RemoveCollider(this);
	mGame->RemoveDoor(mName);
}
