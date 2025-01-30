#pragma once
#include "Component.h"
#include "Actor.h"
#include <functional>

class HealthComponent : public Component
{
private:
	float mHealth = 1000.0f;
	std::function<void(const Vector3&)> mOnDamage;
	std::function<void()> mOnDeath;

public:
	HealthComponent(Actor* owner, float startingHealth);

	// Setters for callbacks
	void SetOnDamageCallback(std::function<void(const Vector3&)> onDamage) { mOnDamage = onDamage; }
	void SetOnDeathCallback(std::function<void()> onDeath) { mOnDeath = onDeath; }

	// Getter for health
	float GetHealth() const { return mHealth; }

	// Check if dead
	bool IsDead() const { return mHealth <= 0.0f; }

	// Take damage
	void TakeDamage(float damageAmount, const Vector3& damageLocation);
};
