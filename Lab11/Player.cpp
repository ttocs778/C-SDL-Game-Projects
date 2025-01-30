#include "Player.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "PlayerMove.h"
#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "PlayerMesh.h"
#include "HealthComponent.h"

Player::Player(Game* game)
: Actor(game)
{
	mPlayerMove = new PlayerMove(this);
	mCamComponent = new CameraComponent(this);
	mPlayerCollision = new CollisionComponent(this);
	mPlayerCollision->SetSize(50.0f, 100.0f, 50.0f);
	mPlayerHealth = new HealthComponent(this, 100.0f);

	mPlayerHealth->SetOnDeathCallback([this] {
		SDL_Log("Player has died. Reloading level...");
		GetGame()->SetNextLevel(GetGame()->GetCurrentLevel());
	});
}

void Player::OnUpdate(float deltaTime)
{
}

void Player::GiveGun()
{
	mHasGun = true;
	mPlayerMesh = new PlayerMesh(GetGame());
}
