#include "Game.h"
#include "AnimatedSprite.h"
#include "Bush.h"
#include "CollisionComponent.h"
#include "EnemyComponent.h"
#include "Effect.h"

Bush::Bush(Game* game)
: Actor(game)
{
	mBushCollision = new CollisionComponent(this);
	mBushCollision->SetSize(BUSH_COLL_SIZE, BUSH_COLL_SIZE);
	mBushEnemy = new EnemyComponent(this);
	mBushEnemy->SetHitPoint(1);
	mBushSprite = new SpriteComponent(this, 150);
	mBushSprite->SetTexture(game->GetTexture("Assets/Bush.png"));
	PathFinder* bushPath = this->GetGame()->GetPathFinder();
	mBushEnemy->SetOnDeathCallBack([this, bushPath]() {
		new Effect(this->GetGame(), this->GetPosition(), "BushDeath", "BushDie.wav");
		bushPath->SetIsBlocked(static_cast<size_t>(this->GetPosition().y / 32),
							   static_cast<size_t>(this->GetPosition().x / 32), false);
	});
}