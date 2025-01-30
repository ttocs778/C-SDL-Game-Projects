#include "PlayerMove.h"
#include "Actor.h"
#include "Math.h"
#include "Renderer.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "CameraComponent.h"
#include "Crosshair.h"
#include "Portal.h"
#include "SegmentCast.h"
#include "Block.h"

PlayerMove::PlayerMove(Actor* owner)
: MoveComponent(owner)
{
	ChangeState(Falling);
	mCrosshair = new Crosshair(this->GetOwner());
}

void PlayerMove::Update(float deltaTime)
{
	if (mCurrentState == OnGround)
	{
		UpdateOnGround(deltaTime);
	}
	else if (mCurrentState == Jump)
	{
		UpdateJump(deltaTime);
	}
	else if (mCurrentState == Falling)
	{
		UpdateFalling(deltaTime);
	}
}
void PlayerMove::ProcessInput(const Uint8* keyState, Uint32 mouseButtons,
							  const Vector2& relativeMouse)
{
	if (keyState[SDL_SCANCODE_W] && !keyState[SDL_SCANCODE_S])
	{
		AddForce(mOwner->GetForward() * MOVE_SCALE);
	}
	else if (keyState[SDL_SCANCODE_S] && !keyState[SDL_SCANCODE_W])
	{
		AddForce(mOwner->GetForward() * -MOVE_SCALE);
	}

	if (keyState[SDL_SCANCODE_A] && !keyState[SDL_SCANCODE_D])
	{
		AddForce(mOwner->GetRight() * -MOVE_SCALE);
	}
	else if (keyState[SDL_SCANCODE_D] && !keyState[SDL_SCANCODE_A])
	{
		AddForce(mOwner->GetRight() * MOVE_SCALE);
	}
	float angularSpeed = relativeMouse.x / 500.0f * Math::Pi * 10.0f;
	SetAngularSpeed(angularSpeed);

	float pitchSpeed = relativeMouse.y / 500.0f * Math::Pi * 10.0f;
	mOwner->GetComponent<CameraComponent>()->SetPitchSpeed(pitchSpeed);

	bool isSpacebarPressed = keyState[SDL_SCANCODE_SPACE];
	if (isSpacebarPressed && mCurrentState == OnGround)
	{
		AddForce(mJumpForce);
		ChangeState(Jump);
	}

	if (mOwner->GetGame()->GetPlayer()->HasGun())
	{
		bool leftMousePressed = mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT);
		bool rightMousePressed = mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		bool rKeyPressed = keyState[SDL_SCANCODE_R];
		if (leftMousePressed && !mIsLeftMousePressed)
		{
			CreatePortal(true);
		}
		mIsLeftMousePressed = leftMousePressed;
		if (rightMousePressed && !mIsRightMousePressed)
		{
			CreatePortal(false);
		}
		mIsRightMousePressed = rightMousePressed;
		if (rKeyPressed && !mIsRPressed)
		{
			ResetPortals();
		}
		mIsRPressed = rKeyPressed;
	}
}

void PlayerMove::PhysicsUpdate(float deltaTime)
{
	mAcceleration = Vector3::Zero;
	mAcceleration = mPendingForces * (1.0f / mMass);
	mVelocity += mAcceleration * deltaTime;
	FixXYVelocity();
	Vector3 newPosition = mOwner->GetPosition() + mVelocity * deltaTime;
	mOwner->SetPosition(newPosition);
	float currentRotation = mOwner->GetRotation();
	float newRotation = currentRotation + mAngularSpeed * deltaTime;
	mOwner->SetRotation(newRotation);
	mPendingForces = Vector3::Zero;
}

void PlayerMove::FixXYVelocity()
{
	Vector2 xyVelocity{mVelocity.x, mVelocity.y};

	if (xyVelocity.Length() >= MAX_SPEED)
	{
		xyVelocity.Normalize();
		xyVelocity *= MAX_SPEED;
	}
	if (mCurrentState == OnGround)
	{
		// Check if mAcceleration.x and xyVelocity.x are opposite signs OR if mAcceleration.x is near zero
		if ((mAcceleration.x * xyVelocity.x < 0.0f) || Math::NearlyZero(mAcceleration.x))
		{
			xyVelocity.x *= BRAKING_FACTOR;
		}

		// Check if mAcceleration.y and xyVelocity.y are opposite signs OR if mAcceleration.y is near zero
		if ((mAcceleration.y * xyVelocity.y < 0.0f) || Math::NearlyZero(mAcceleration.y))
		{
			xyVelocity.y *= BRAKING_FACTOR;
		}
	}
	mVelocity.x = xyVelocity.x;
	mVelocity.y = xyVelocity.y;
}

CollSide PlayerMove::FixCollision(CollisionComponent* self, CollisionComponent* collider)
{
	Vector3 offset;
	CollSide collisionSide = self->GetMinOverlap(collider, offset);
	if (collisionSide != CollSide::None)
	{
		Vector3 position = mOwner->GetPosition();
		position += offset;
		mOwner->SetPosition(position);
	}
	return collisionSide;
}

void PlayerMove::UpdateOnGround(float deltaTime)
{
	PhysicsUpdate(deltaTime);
	bool onGround = false; // Flag to check if player is on ground

	for (auto collider : mOwner->GetGame()->GetCollider())
	{
		CollSide side = FixCollision(mOwner->GetComponent<CollisionComponent>(),
									 collider->GetComponent<CollisionComponent>());

		if (side == CollSide::Top)
		{
			onGround = true;
		}
	}
	if (!onGround)
	{
		ChangeState(Falling);
	}
}

void PlayerMove::UpdateJump(float deltaTime)
{
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);
	for (auto collider : mOwner->GetGame()->GetCollider())
	{
		if (FixCollision(mOwner->GetComponent<CollisionComponent>(),
						 collider->GetComponent<CollisionComponent>()) == CollSide::Bottom)
		{
			mVelocity.z = 0.0f;
		}
	}
	if (mVelocity.z <= 0.0f)
	{
		ChangeState(Falling);
	}
}

void PlayerMove::UpdateFalling(float deltaTime)
{
	mAcceleration = Vector3::Zero;
	AddForce(mGravity);
	PhysicsUpdate(deltaTime);
	for (auto collider : mOwner->GetGame()->GetCollider())
	{
		if (FixCollision(mOwner->GetComponent<CollisionComponent>(),
						 collider->GetComponent<CollisionComponent>()) == CollSide::Top &&
			mVelocity.z <= 0.0f)
		{
			mVelocity.z = 0.0f;
			ChangeState(OnGround);
		}
	}
}

void PlayerMove::CreatePortal(bool isBlue)
{
	Renderer* renderer = mOwner->GetGame()->GetRenderer();
	Vector3 nearPoint = renderer->Unproject(Vector3(0.0f, 0.0f, 0.0f));
	Vector3 farPoint = renderer->Unproject(Vector3(0.0f, 0.0f, 1.0f));
	Vector3 direction = Vector3::Normalize(farPoint - nearPoint);
	LineSegment segment(nearPoint, nearPoint + direction * 1000.0f);
	CastInfo castInfo;
	std::vector<Actor*> actors = mOwner->GetGame()->GetActor();
	if (SegmentCast(actors, segment, castInfo, mOwner))
	{
		Block* block = dynamic_cast<Block*>(castInfo.mActor);
		if (!block)
			return;

		Portal* currentPortal = isBlue ? mOwner->GetGame()->GetBluePortal()
									   : mOwner->GetGame()->GetOrangePortal();
		if (currentPortal)
		{
			currentPortal->SetState(ActorState::Destroy);
		}
		Portal* newPortal = new Portal(mOwner->GetGame(), isBlue);
		newPortal->SetPosition(castInfo.mPoint);

		Vector3 desiredFacing = castInfo.mNormal;
		Vector3 originalFacing = Vector3::UnitX;

		float dotProduct = Vector3::Dot(originalFacing, desiredFacing);
		if (Math::NearlyEqual(dotProduct, 1.0f))
		{
			newPortal->SetQuat(Quaternion::Identity);
		}
		else if (Math::NearlyEqual(dotProduct, -1.0f))
		{
			newPortal->SetQuat(Quaternion(Vector3::UnitZ, Math::Pi));
		}
		else
		{
			Vector3 rotationAxis = Vector3::Cross(originalFacing, desiredFacing);
			rotationAxis.Normalize();
			float rotationAngle = Math::Acos(dotProduct);
			newPortal->SetQuat(Quaternion(rotationAxis, rotationAngle));
		}
		if (isBlue)
		{
			mOwner->GetGame()->SetBluePortal(newPortal);
		}
		else
		{
			mOwner->GetGame()->SetOrangePortal(newPortal);
		}
		UpdateCrosshairState();
	}
}

void PlayerMove::UpdateCrosshairState()
{

	bool hasBluePortal = (mOwner->GetGame()->GetBluePortal() != nullptr);
	bool hasOrangePortal = (mOwner->GetGame()->GetOrangePortal() != nullptr);

	if (hasBluePortal && hasOrangePortal)
	{
		mCrosshair->SetState(CrosshairState::BothFill);
	}
	else if (hasBluePortal)
	{
		mCrosshair->SetState(CrosshairState::BlueFill);
	}
	else if (hasOrangePortal)
	{
		mCrosshair->SetState(CrosshairState::OrangeFill);
	}
	else
	{
		mCrosshair->SetState(CrosshairState::Default);
	}
}

void PlayerMove::ResetPortals()
{
	Game* game = mOwner->GetGame();
	if (game->GetBluePortal())
	{
		game->GetBluePortal()->SetState(ActorState::Destroy);
		game->SetBluePortal(nullptr);
	}

	if (game->GetOrangePortal())
	{
		game->GetOrangePortal()->SetState(ActorState::Destroy);
		game->SetOrangePortal(nullptr);
	}

	mCrosshair->SetState(CrosshairState::Default);
}
