#include "Player.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "PlayerMove.h"
#include "CameraComponent.h"
#include "PlayerUI.h"

Player::Player(Game* game)
: Actor(game)
{
	SetScale(0.75f);
	mMc = new MeshComponent(this);
	mMc->SetMesh(game->GetRenderer()->GetMesh("Assets/Kart.gpmesh"));
	mPlayerMove = new PlayerMove(this);
	mCamComponent = new CameraComponent(this);
	mCamComponent->SnapToIdeal();
	mPlayerUi = new PlayerUI(this);
}

void Player::OnUpdate(float deltaTime)
{
}
