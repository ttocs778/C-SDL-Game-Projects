#include "CollisionComponent.h"
#include "Actor.h"
#include <algorithm>

CollisionComponent::CollisionComponent(class Actor* owner)
: Component(owner)
, mWidth(0.0f)
, mHeight(0.0f)
, mDepth(0.0f)
{
}

CollisionComponent::~CollisionComponent()
{
}

bool CollisionComponent::Intersect(const CollisionComponent* other) const
{
	Vector3 thisMin = GetMin();
	Vector3 thisMax = GetMax();
	Vector3 otherMin = other->GetMin();
	Vector3 otherMax = other->GetMax();

	bool noIntersection = thisMax.x < otherMin.x || thisMax.y < otherMin.y ||
						  thisMax.z < otherMin.z || otherMax.x < thisMin.x ||
						  otherMax.y < thisMin.y || otherMax.z < thisMin.z;

	return !noIntersection;
}

Vector3 CollisionComponent::GetMin() const
{
	Vector3 v = mOwner->GetPosition();
	v.x -= mDepth * mOwner->GetScale().x / 2.0f;
	v.y -= mWidth * mOwner->GetScale().y / 2.0f;
	v.z -= mHeight * mOwner->GetScale().z / 2.0f;
	return v;
}

Vector3 CollisionComponent::GetMax() const
{
	Vector3 v = mOwner->GetPosition();
	v.x += mDepth * mOwner->GetScale().x / 2.0f;
	v.y += mWidth * mOwner->GetScale().y / 2.0f;
	v.z += mHeight * mOwner->GetScale().z / 2.0f;
	return v;
}

Vector3 CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

CollSide CollisionComponent::GetMinOverlap(const CollisionComponent* other, Vector3& offset) const
{
	// Start with zero offset
	offset = Vector3::Zero;

	// Early exit if no intersection
	if (!Intersect(other))
	{
		return CollSide::None;
	}

	// Calculate overlap distances along each axis
	float frontDist = other->GetMax().x - GetMin().x;
	float backDist = GetMax().x - other->GetMin().x;
	float leftDist = GetMax().y - other->GetMin().y;
	float rightDist = other->GetMax().y - GetMin().y;
	float topDist = other->GetMax().z - GetMin().z;
	float bottomDist = GetMax().z - other->GetMin().z;

	CollSide minSide = CollSide::Front;
	offset = Vector3(frontDist, 0.0f, 0.0f);

	if (Math::Abs(backDist) < Math::Abs(offset.x))
	{
		minSide = CollSide::Back;
		offset = Vector3(-backDist, 0.0f, 0.0f);
	}

	// Check Left
	if (Math::Abs(leftDist) < Math::Abs(offset.x))
	{
		minSide = CollSide::Left;
		offset = Vector3(0.0f, -leftDist, 0.0f);
	}

	// Check Right
	if (Math::Abs(rightDist) < Math::Abs(offset.y ? offset.y : offset.x))
	{
		minSide = CollSide::Right;
		offset = Vector3(0.0f, rightDist, 0.0f);
	}

	// Check Top
	if (Math::Abs(topDist) < Math::Abs(offset.z ? offset.z : (offset.y ? offset.y : offset.x)))
	{
		minSide = CollSide::Top;
		offset = Vector3(0.0f, 0.0f, topDist);
	}

	// Check Bottom
	if (Math::Abs(bottomDist) < Math::Abs(offset.z ? offset.z : (offset.y ? offset.y : offset.x)))
	{
		minSide = CollSide::Bottom;
		offset = Vector3(0.0f, 0.0f, -bottomDist);
	}

	return minSide;
}
