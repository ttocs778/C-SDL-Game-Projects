#include "EnergyCube.h"
#include "CollisionComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "Door.h"

EnergyCube::EnergyCube(Game* game)
: Actor(game)
{
	mEnergyCubeCollision = new CollisionComponent(this);
	mEnergyCubeMesh = new MeshComponent(this);
	Mesh* energyCubeMesh = game->GetRenderer()->GetMesh("Assets/Meshes/EnergyCube.gpmesh");
	mEnergyCubeMesh->SetMesh(energyCubeMesh);
	mEnergyCubeCollision->SetSize(25.0f, 25.0f, 25.0f);
	game->AddCollider(this);
}

EnergyCube::~EnergyCube()
{
	mGame->RemoveCollider(this);
}