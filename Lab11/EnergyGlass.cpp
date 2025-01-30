#include "EnergyGlass.h"
#include "CollisionComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "Door.h"

EnergyGlass::EnergyGlass(Game* game)
: Actor(game)
{
	mEnergyGlassCollision = new CollisionComponent(this);
	mEnergyGlassMesh = new MeshComponent(this, true);
	Mesh* energyGlassMesh = game->GetRenderer()->GetMesh("Assets/Meshes/Cube.gpmesh");
	mEnergyGlassMesh->SetMesh(energyGlassMesh);
	mEnergyGlassMesh->SetTextureIndex(17);
	mEnergyGlassCollision->SetSize(1.0f, 1.0f, 1.0f);
	game->AddCollider(this);
}

EnergyGlass::~EnergyGlass()
{
	mGame->RemoveCollider(this);
}