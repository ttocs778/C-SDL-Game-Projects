#include "Game.h"
#include "Player.h"
#include "PlayerMove.h"
#include "AnimatedSprite.h"
#include "CollisionComponent.h"

Player::Player(Game* game)
: Actor(game)
{
	mPlayerAnimation = new AnimatedSprite(this, 200);
	mPlayerAnimation->LoadAnimations("Assets/Link");
	mPlayerAnimation->SetAnimation("StandDown");
	mPlayerCollision = new CollisionComponent(this);
	mPlayerCollision->SetSize(PLAYER_COLL_SIZE, PLAYER_COLL_SIZE);
	mPlayerMove = new PlayerMove(this);
}