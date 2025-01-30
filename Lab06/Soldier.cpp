#include "Game.h"
#include "AnimatedSprite.h"
#include "Soldier.h"
#include "CollisionComponent.h"
#include "PathFinder.h"
#include "SoldierAI.h"
#include "EnemyComponent.h"
#include "Effect.h"

Soldier::Soldier(Game* game, PathNode* start, PathNode* end)
: Actor(game)
{
	mSoldierAnimation = new AnimatedSprite(this, 200);
	mSoldierAnimation->LoadAnimations("Assets/Soldier");
	mSoldierAnimation->SetAnimation("WalkDown");
	mSoldierAnimation->SetAnimFPS(5.0f);
	mSoldierCollision = new CollisionComponent(this);
	mSoldierCollision->SetSize(SOLDIER_COLL_SIZE, SOLDIER_COLL_SIZE);
	mSoldierEnemy = new EnemyComponent(this);
	mSoldierEnemy->SetHitPoint(2);
	SoldierAI* soldierAi = new SoldierAI(this);
	mSoldierEnemy->SetOnDamageCallBack([this, soldierAi]() {
		new Effect(this->GetGame(), this->GetPosition(), "Hit", "EnemyHit.wav");
		soldierAi->Stun();
	});
	mSoldierEnemy->SetOnDeathCallBack([this, game]() {
		new Effect(game, this->GetPosition(), "Death", "EnemyDie.wav");
	});
	soldierAi->Setup(start, end);
}