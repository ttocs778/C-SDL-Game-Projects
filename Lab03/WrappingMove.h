#pragma once
#include "MoveComponent.h"
#include "Math.h"
#include "Actor.h"

class WrappingMove : public MoveComponent
{
public:
	WrappingMove(Actor* owner, int updateOrder = 10);
	void Update(float deltaTime) override;
	void SetDirection(const Vector2& dir);
	const Vector2& GetDirection() const { return mDirection; }

private:
	Vector2 mDirection;
};
