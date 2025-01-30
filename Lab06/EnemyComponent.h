#pragma once
#include "Component.h"
#include <functional>

class EnemyComponent : public Component
{

public:
	EnemyComponent(class Actor* owner);
	~EnemyComponent();
	class CollisionComponent* GetEnemyColl() const { return mEnemyCollision; }
	int GetHitPoint() const { return mHitPoint; }
	void SetHitPoint(int hitPoint) { mHitPoint = hitPoint; }
	void TakeDamage();
	void Update(float deltaTime) override;
	void SetOnDamageCallBack(std::function<void()> onDamage) { mOnDamage = onDamage; }
	void SetOnDeathCallBack(std::function<void()> onDeath) { mOnDeath = onDeath; }

private:
	class CollisionComponent* mEnemyCollision;
	int mHitPoint = 0;
	bool mIsVulnerable = false;
	float mVulnerableTime = 0.0f;
	float mVulnerableDuration = 0.25f;
	std::function<void()> mOnDamage;
	std::function<void()> mOnDeath;
};