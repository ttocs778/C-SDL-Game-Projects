#include "Player.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "CollisionComponent.h"
#include "PlayerMove.h"
#include "HUD.h"

Player::Player(Game* game)
: Actor(game)
{
	SetScale(2.0);
	mMc = new MeshComponent(this);
	mMc->SetMesh(game->GetRenderer()->GetMesh("Assets/Arwing.gpmesh"));
	mPlayerColl = new CollisionComponent(this);
	mPlayerColl->SetSize(40.0f, 25.0f, 15.0f);
	mPlayerMove = new PlayerMove(this);
	mHud = new HUD(this);
	game->PlayShipLoop();
}

void Player::OnUpdate(float deltaTime)
{
	if (mIsInvulnerable)
	{
		mInvulnerabilityTimer -= deltaTime;
		if (mInvulnerabilityTimer <= 0.0f)
		{
			mIsInvulnerable = false;
		}
	}
}

void Player::TakeDamage()
{
	if (mIsInvulnerable)
	{
		return;
	}
	mGame->GetAudio()->PlaySound("ShipHit.wav");
	mShieldTrack--;
	mIsInvulnerable = true;
	mInvulnerabilityTimer = 1.0f;
	if (mShieldTrack <= 0)
	{
		mGame->GetAudio()->PlaySound("ShipDie.wav");
		mGame->StopShipLoop();
		mGame->StopDamageAlert();
		SetState(ActorState::Paused);
	}
	if (mShieldTrack == 1)
	{
		mGame->PlayDamageAlert();
	}
}
