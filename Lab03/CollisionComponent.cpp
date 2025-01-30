#include "CollisionComponent.h"
#include "Actor.h"
#include <algorithm>

CollisionComponent::CollisionComponent(class Actor* owner)
: Component(owner)
, mWidth(0.0f)
, mHeight(0.0f)
{
}

CollisionComponent::~CollisionComponent()
{
}

bool CollisionComponent::Intersect(const CollisionComponent* other) const
{
	Vector2 aMin = GetMin();
	Vector2 aMax = GetMax();
	Vector2 bMin = other->GetMin();
	Vector2 bMax = other->GetMax();
	return (aMax.x >= bMin.x && bMax.x >= aMin.x && aMax.y >= bMin.y && bMax.y >= aMin.y);
}

Vector2 CollisionComponent::GetMin() const
{
	Vector2 ownerPos = mOwner->GetPosition();
	float ownerScale = mOwner->GetScale();

	Vector2 min;
	min.x = ownerPos.x - (mWidth * ownerScale) / 2.0f;
	min.y = ownerPos.y - (mHeight * ownerScale) / 2.0f;

	return min;
}

Vector2 CollisionComponent::GetMax() const
{
	Vector2 ownerPos = mOwner->GetPosition();
	float ownerScale = mOwner->GetScale();

	Vector2 max;
	max.x = ownerPos.x + (mWidth * ownerScale) / 2.0f;
	max.y = ownerPos.y + (mHeight * ownerScale) / 2.0f;

	return max;
}

const Vector2& CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

CollSide CollisionComponent::GetMinOverlap(const CollisionComponent* other, Vector2& offset) const
{
	// If there's no intersection, return None
	if (!Intersect(other))
	{
		return CollSide::None;
	}

	// Calculate distances from the sides of this to the sides of the other
	float leftDist = other->GetMin().x - GetMax().x;
	float rightDist = other->GetMax().x - GetMin().x;
	float topDist = other->GetMin().y - GetMax().y;
	float bottomDist = other->GetMax().y - GetMin().y;

	// Start with the left side as the minimum overlap
	CollSide minSide = CollSide::Left;
	offset.x = leftDist;
	offset.y = 0.0f;

	// Check if the right side overlap is smaller
	if (Math::Abs(rightDist) < Math::Abs(leftDist))
	{
		minSide = CollSide::Right;
		offset.x = rightDist;
		offset.y = 0.0f;
	}

	// Check if the top side overlap is smaller
	if (Math::Abs(topDist) < Math::Abs(offset.x))
	{
		minSide = CollSide::Top;
		offset.x = 0.0f;
		offset.y = topDist;
	}

	// Check if the bottom side overlap is smaller
	if (Math::Abs(bottomDist) < Math::Abs(offset.y ? offset.y : offset.x))
	{
		minSide = CollSide::Bottom;
		offset.x = 0.0f;
		offset.y = bottomDist;
	}

	// Return the side with the minimum overlap
	return minSide;
}
