#include "HealthComponent.h"

HealthComponent::HealthComponent(Actor* owner, float startingHealth)
: Component(owner)
, mHealth(startingHealth)
{
}

void HealthComponent::TakeDamage(float damageAmount, const Vector3& damageLocation)
{
	if (IsDead())
	{
		return; // Already dead, no need to process further
	}

	mHealth -= damageAmount;

	// Call the OnDamage callback if set
	if (mOnDamage)
	{
		mOnDamage(damageLocation);
	}

	// Check if the health is zero or below
	if (IsDead())
	{
		// Call the OnDeath callback if set
		if (mOnDeath)
		{
			mOnDeath();
		}
	}
}
