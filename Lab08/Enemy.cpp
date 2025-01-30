#include "Enemy.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Mesh.h"
#include "EnemyMove.h"
#include "Game.h"

Enemy::Enemy(Game* game)
: Actor(game)
{
	SetScale(0.75f);
	mEnemyMc = new MeshComponent(this);
	mEnemyMc->SetTextureIndex(6);
	mEnemyMc->SetMesh(game->GetRenderer()->GetMesh("Assets/Kart.gpmesh"));
	mEnemyMove = new EnemyMove(this, "Assets/HeightMap/Path.csv");
}