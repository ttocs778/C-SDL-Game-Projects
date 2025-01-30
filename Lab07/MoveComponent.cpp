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
	float currentRotation = mOwner->GetRotation();
	float newRotation = currentRotation + mAngularSpeed * deltaTime;

	// Wrap the rotation if it exceeds 2π radians (360 degrees)
	if (newRotation > Math::TwoPi)
	{
		newRotation -= Math::TwoPi;
	}
	else if (newRotation < 0.0f)
	{
		newRotation += Math::TwoPi;
	}

	// Set the new rotation back to the actor
	mOwner->SetRotation(newRotation);

	if (mForwardSpeed != 0.0f)
	{
		Vector3 currentPosition = mOwner->GetPosition();
		Vector3 forwardVector = mOwner->GetForward();
		Vector3 newPosition = currentPosition + forwardVector * mForwardSpeed * deltaTime;
		mOwner->SetPosition(newPosition);
	}
}