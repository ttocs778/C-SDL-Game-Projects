#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(class Actor* owner)
: Component(owner, 50)
, mAngularSpeed(0.0f)
, mForwardSpeed(0.0f)
{
}

void MoveComponent::Update(float deltaTime)
{
	// Update the rotation (no need to wrap it)
	if (mAngularSpeed != 0.0f)
	{
		float newRotation = mOwner->GetRotation() + mAngularSpeed * deltaTime;
		mOwner->SetRotation(newRotation);
	}

	// Update the position if there's forward speed
	if (mForwardSpeed != 0.0f)
	{
		Vector2 currentPosition = mOwner->GetPosition();
		Vector2 forwardVector = mOwner->GetForward();
		Vector2 newPosition = currentPosition + forwardVector * mForwardSpeed * deltaTime;
		mOwner->SetPosition(newPosition);
	}
}