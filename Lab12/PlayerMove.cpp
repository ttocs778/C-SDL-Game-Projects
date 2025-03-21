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
#include "HealthComponent.h"

PlayerMove::PlayerMove(Actor* owner)
: MoveComponent(owner)
{
	ChangeState(OnGround);
	mCrosshair = new Crosshair(this->GetOwner());
	mFootstepSound = mOwner->GetGame()->GetAudio()->PlaySound("FootstepLoop.ogg", true, mOwner);
	mOwner->GetGame()->GetAudio()->PauseSound(mFootstepSound);
}

PlayerMove::~PlayerMove()
{
	mOwner->GetGame()->GetAudio()->StopSound(mFootstepSound);
}

void PlayerMove::Update(float deltaTime)
{
	HealthComponent* health = mOwner->GetComponent<HealthComponent>();
	if (health && health->IsDead())
	{
		Player* player = dynamic_cast<Player*>(mOwner);
		if (player && player->GetDeathSound().IsValid() &&
			mOwner->GetGame()->GetAudio()->GetSoundState(player->GetDeathSound()) !=
				SoundState::Stopped)
		{
			return;
		}

		mOwner->GetGame()->SetNextLevel(mOwner->GetGame()->GetCurrentLevel());
		return;
	}
	// Detect state changes for specific sounds
	if (mPreviousState != mCurrentState)
	{
		HandleStateChange();
		mPreviousState = mCurrentState;
	}

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
	if (mOwner->GetPosition().z <= FALL_MAX)
	{
		mOwner->GetGame()->SetNextLevel(mOwner->GetGame()->GetCurrentLevel());
		return;
	}
	if (mOwner->GetPosition().z <= FALL_MAX)
	{
		// Get the player's HealthComponent
		HealthComponent* health = mOwner->GetComponent<HealthComponent>();
		if (health)
		{
			health->TakeDamage(Math::Infinity, mOwner->GetPosition());
		}
		return;
	}
	if (mCurrentState == OnGround && mVelocity.Length() > 50.0f)
	{
		mOwner->GetGame()->GetAudio()->ResumeSound(mFootstepSound);
	}
	else
	{
		mOwner->GetGame()->GetAudio()->PauseSound(mFootstepSound);
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
			mPortalCloseSound = GetGame()->GetAudio()->PlaySound("PortalClose.ogg");
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
	if (mVelocity.z <= -1000.0f)
		mVelocity.z = -1000.0f;
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

	if (!mFallingByTeleport && xyVelocity.Length() >= MAX_SPEED)
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
	if (UpdatePortalTeleport(deltaTime))
	{
		return;
	}
	bool onGround = false;
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
	if (UpdatePortalTeleport(deltaTime))
	{
		return;
	}
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
	if (UpdatePortalTeleport(deltaTime))
	{
		return;
	}
	for (auto collider : mOwner->GetGame()->GetCollider())
	{
		if (FixCollision(mOwner->GetComponent<CollisionComponent>(),
						 collider->GetComponent<CollisionComponent>()) == CollSide::Top &&
			mVelocity.z <= 0.0f)
		{
			mVelocity.z = 0.0f;
			ChangeState(OnGround);
			mFallingByTeleport = false;
		}
	}
}

void PlayerMove::CreatePortal(bool isBlue)
{
	AudioSystem* audio = mOwner->GetGame()->GetAudio();
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
		{
			audio->PlaySound("PortalFail.ogg");
			return;
		}

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
		newPortal->SetupCollisionComponent(castInfo.mNormal);
		if (isBlue)
		{
			mOwner->GetGame()->SetBluePortal(newPortal);
			audio->PlaySound("PortalShootBlue.ogg");
		}
		else
		{
			mOwner->GetGame()->SetOrangePortal(newPortal);
			audio->PlaySound("PortalShootOrange.ogg");
		}
		newPortal->Update(0.0f);
		UpdateCrosshairState();
	}
	else
	{
		audio->PlaySound("PortalFail.ogg");
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

bool PlayerMove::UpdatePortalTeleport(float deltaTime)
{
	static float timeSinceLastTeleport = 0.0f;
	timeSinceLastTeleport += deltaTime;

	Portal* bluePortal = mOwner->GetGame()->GetBluePortal();
	Portal* orangePortal = mOwner->GetGame()->GetOrangePortal();
	if (!bluePortal || !orangePortal || timeSinceLastTeleport < TELEPORT_GAP)
	{
		return false;
	}
	CollisionComponent* playerCollider =
		mOwner->GetGame()->GetPlayer()->GetComponent<CollisionComponent>();
	if (playerCollider->Intersect(bluePortal->GetComponent<CollisionComponent>()))
	{
		CalTeleport(bluePortal, orangePortal);
		timeSinceLastTeleport = 0.0f;
		return true;
	}
	else if (playerCollider->Intersect(orangePortal->GetComponent<CollisionComponent>()))
	{
		CalTeleport(orangePortal, bluePortal);
		timeSinceLastTeleport = 0.0f;
		return true;
	}

	return false;
}

void PlayerMove::CalTeleport(Portal* entry, Portal* exit)
{
	mTeleportSound = mOwner->GetGame()->GetAudio()->PlaySound("PortalTeleport.ogg");
	// Determine if entry or exit portals face along ±Z-axis
	bool entryAlongZ = Math::NearlyEqual(entry->GetQuatForward().z, 1.0f) ||
					   Math::NearlyEqual(entry->GetQuatForward().z, -1.0f);

	bool exitAlongZ = Math::NearlyEqual(exit->GetQuatForward().z, 1.0f) ||
					  Math::NearlyEqual(exit->GetQuatForward().z, -1.0f);

	if (!exitAlongZ)
	{
		Vector3 newFacing;

		if (entryAlongZ)
		{
			newFacing = exit->GetQuatForward();
		}
		else
		{
			Vector3 playerForward = mOwner->GetForward();
			newFacing = entry->GetPortalOutVector(playerForward, exit, 0.0f);
		}
		newFacing.z = 0.0f;
		newFacing.Normalize();

		Vector3 unitX = Vector3::UnitX;
		float dot = Vector3::Dot(unitX, newFacing);
		dot = Math::Clamp(dot, -1.0f, 1.0f);
		float angle = Math::Acos(dot);
		Vector3 cross = Vector3::Cross(unitX, newFacing);
		if (cross.z < 0.0f)
		{
			angle = -angle;
		}
		mOwner->SetRotation(angle);
	}

	float currentVelocityLength = mVelocity.Length();
	float newVelocityLength = Math::Max(1.5f * currentVelocityLength, VEL_LENGTH_CAP);
	Vector3 velocityDirection;

	if (entryAlongZ || exitAlongZ)
	{
		velocityDirection = exit->GetQuatForward();
	}
	else
	{
		Vector3 velocityDirectionOriginal = Vector3::Normalize(mVelocity);
		velocityDirection = entry->GetPortalOutVector(velocityDirectionOriginal, exit, 0.0f);
	}
	velocityDirection.Normalize();
	mVelocity = velocityDirection * newVelocityLength;
	Vector3 baseTeleportPoint;

	if (entryAlongZ || exitAlongZ)
	{
		baseTeleportPoint = exit->GetPosition();
	}
	else
	{
		Vector3 playerPosition = mOwner->GetPosition();
		baseTeleportPoint = entry->GetPortalOutVector(playerPosition, exit, 1.0f);
	}
	Vector3 offset = exit->GetQuatForward() * LAUNCHING_DISTANCE;
	Vector3 newPosition = baseTeleportPoint + offset;
	mOwner->SetPosition(newPosition);
	ChangeState(Falling);
	mFallingByTeleport = true;
}

void PlayerMove::HandleStateChange()
{
	AudioSystem* audio = mOwner->GetGame()->GetAudio();

	if (mCurrentState == Jump)
	{
		mJumpSound = audio->PlaySound("Jump.ogg");
	}
	else if (mCurrentState == OnGround && mPreviousState == Falling)
	{
		mLandSound = audio->PlaySound("Land.ogg");
	}
}