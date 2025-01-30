#include "EnemyComponent.h"
#include "Actor.h"
#include "CollisionComponent.h"
#include "Game.h"

EnemyComponent::EnemyComponent(Actor* owner)
: Component(owner)
{
	owner->GetGame()->GetEnemies().push_back(this);
	mEnemyCollision = owner->GetComponent<CollisionComponent>();
}

EnemyComponent::~EnemyComponent()
{
	auto& enemies = mOwner->GetGame()->GetEnemies();
	auto it = std::find(enemies.begin(), enemies.end(), this);
	if (it != enemies.end())
	{
		enemies.erase(it);
	}
}

void EnemyComponent::Update(float deltaTime)
{
	if (mIsVulnerable)
	{
		mVulnerableTime += deltaTime;
		if (mVulnerableTime >= mVulnerableDuration)
		{
			mIsVulnerable = false;
		}
	}
}

void EnemyComponent::TakeDamage()
{
	if (mIsVulnerable)
	{
		return;
	}

	mHitPoint -= 1;
	mIsVulnerable = true;
	mVulnerableTime = 0.0f;

	if (mHitPoint <= 0)
	{
		mOwner->SetState(ActorState::Destroy);

		// Call the death callback, if set
		if (mOnDeath)
		{
			mOnDeath();
		}
	}
	else
	{
		// Call the damage callback, if set
		if (mOnDamage)
		{
			mOnDamage();
		}
	}
}
