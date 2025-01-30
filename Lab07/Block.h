#pragma once
#include "Actor.h"

class Block : public Actor
{
public:
	enum BlockType
	{
		Regular,
		Exploding
	};
	Block(Game* game, BlockType type);
	~Block();
	void SetExplodCheck(bool isExploding) { mIsExploding = isExploding; }
	bool GetExplodCheck() const { return mIsExploding; }
	void TriggerExplosion();
	void OnUpdate(float deltaTime) override;

private:
	class MeshComponent* mMc;
	class CollisionComponent* mBlockColl;
	bool mIsExploding = false;
	bool mHasExploded = false;
};