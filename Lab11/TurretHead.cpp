#include "TurretHead.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Game.h"
#include "Math.h"
#include "HealthComponent.h"
#include "Random.h"
#include "CollisionComponent.h"
#include "TurretBase.h"

TurretHead::TurretHead(Game* game, Actor* parent)
: Actor(game, parent)
{
	mTurretMesh = new MeshComponent(this);
	SetScale(0.75f);
	SetPosition(Vector3(0.0f, 0.0f, 18.75f));
	mTurretMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/TurretHead.gpmesh"));
	mActorLaser = new Actor(game, this);
	mActorLaser->SetPosition(Vector3(0.0f, 0.0f, 10.0f));
	mLaser = new LaserComponent(mActorLaser);
	mLaser->SetIgnoreActor(parent);
}

void TurretHead::OnUpdate(float deltaTime)
{

	if (mTeleportCooldown > 0.0f)
	{
		mTeleportCooldown -= deltaTime;
	}
	if (CheckForPortalTeleport())
	{
		mTurretState = TurretState::Falling;
		return;
	}
	mStateTimer += deltaTime;
	CheckForTarget();

	switch (mTurretState)
	{
	case TurretState::Idle:
		UpdateIdle(deltaTime);
		break;

	case TurretState::Priming:
		UpdatePriming(deltaTime);
		break;

	case TurretState::Falling:
		UpdateFalling(deltaTime);
		break;

	case TurretState::Dead:
		UpdateDead(deltaTime);
		break;

	case TurretState::Search:
		UpdateSearch(deltaTime);
		break;

	case TurretState::Firing:
		UpdateFiring(deltaTime);
		break;
	}
}
void TurretHead::UpdateIdle(float deltaTime)
{
	SetQuat(Quaternion::Identity);
	if (mAcquiredTarget)
	{
		mTurretState = TurretState::Priming;
		mStateTimer = 0.0f;
	}
}

void TurretHead::UpdatePriming(float deltaTime)
{
	if (mLaser->GetLastHitActor() == nullptr || mLaser->GetLastHitActor() != mAcquiredTarget)
	{
		mTurretState = TurretState::Search;
		return;
	}

	if (mStateTimer >= PRIMING_TIME)
	{
		mTurretState = TurretState::Firing;
		mStateTimer = 0.0f;
		mFireCooldown = FIRE_COOLDOWN;
	}
}

void TurretHead::UpdateSearch(float deltaTime)
{
	// Increment the cycle timer
	mCycleTimer += deltaTime;

	// Return to Idle if the turret has been searching for more than SEARCH_TIMEOUT
	if (mStateTimer > SEARCH_TIMEOUT)
	{
		mStateTimer = 0.0f;		   // Reset timer
		mCycleTimer = 0.0f;		   // Reset cycle timer
		mReturningToCenter = true; // Start interpolating back to center
	}

	// Check if a target is acquired
	if (mAcquiredTarget)
	{
		mTurretState = TurretState::Priming;
		mStateTimer = 0.0f; // Reset timer
		mCycleTimer = 0.0f; // Reset cycle timer
		mReturningToCenter = false;
		return;
	}

	// Calculate a new target on the circle if necessary
	if (!mHasTarget || mCycleTimer > 1.0f)
	{
		// Reset timer for a new cycle
		mCycleTimer = 0.0f;

		// Calculate the target point
		Vector3 forward = Vector3::UnitX * FORWARD_DISTANCE;
		Vector3 side = Vector3::UnitY * Random::GetFloatRange(-SIDE_DISTANCE, SIDE_DISTANCE);
		Vector3 up = Vector3::UnitZ * Random::GetFloatRange(-UP_DISTANCE, UP_DISTANCE);

		mTargetPoint = GetWorldPosition() + forward + side + up;

		// Calculate rotation quaternion to face the target point
		Vector3 direction = mTargetPoint - GetWorldPosition();
		direction.Normalize();

		Vector3 defaultForward = Vector3::UnitX;
		float dot = Vector3::Dot(defaultForward, direction);

		if (Math::NearlyZero(dot - 1.0f))
		{
			mTargetRotation = Quaternion::Identity; // No rotation needed
		}
		else if (Math::NearlyZero(dot + 1.0f))
		{
			mTargetRotation = Quaternion(Vector3::UnitZ, Math::Pi); // Opposite direction
		}
		else
		{
			Vector3 axis = Vector3::Cross(defaultForward, direction);
			axis.Normalize();
			float angle = Math::Acos(dot);
			mTargetRotation = Quaternion(axis, angle);
		}

		mHasTarget = true;
	}

	// Handle interpolation phases
	if (mReturningToCenter)
	{
		// Smoothly interpolate back to center
		float progress = mCycleTimer / 1.0f; // Duration to return to center
		Quaternion currentRotation = Quaternion::Slerp(GetQuat(), Quaternion::Identity, progress);
		SetQuat(currentRotation);

		// Stop interpolating once back at center
		if (progress >= 1.0f)
		{
			mReturningToCenter = false;
			mTurretState = TurretState::Idle;
		}
	}
	else if (mCycleTimer <= 0.5f)
	{
		// Interpolate towards the target
		float progress = mCycleTimer / 0.5f;
		Quaternion currentRotation = Quaternion::Slerp(Quaternion::Identity, mTargetRotation,
													   progress);
		SetQuat(currentRotation);
	}
	else if (mCycleTimer > 0.5f && mCycleTimer <= 1.0f)
	{
		// Interpolate back to center
		float progress = (mCycleTimer - 0.5f) / 0.5f;
		Quaternion currentRotation = Quaternion::Slerp(mTargetRotation, Quaternion::Identity,
													   progress);
		SetQuat(currentRotation);
	}
}

void TurretHead::UpdateFiring(float deltaTime)
{
	if (!mAcquiredTarget || mLaser->GetLastHitActor() != mAcquiredTarget)
	{
		mTurretState = TurretState::Search;
		mStateTimer = 0.0f;
		return;
	}
	HealthComponent* targetHealth = mAcquiredTarget->GetComponent<HealthComponent>();
	if (!targetHealth || targetHealth->IsDead())
	{
		mTurretState = TurretState::Search;
		mStateTimer = 0.0f;
		return;
	}

	mFireCooldown -= deltaTime;

	if (mFireCooldown <= 0.0f)
	{
		targetHealth->TakeDamage(DAMAGE_PER_SHOT, GetWorldPosition());
		mFireCooldown = 0.05f;
	}
}

void TurretHead::UpdateFalling(float deltaTime)
{
	Actor* parent = GetParent();
	if (!parent)
	{
		return;
	}

	Vector3 newPosition = parent->GetPosition() + mFallVelocity * deltaTime;
	parent->SetPosition(newPosition);

	if (CheckForPortalTeleport())
	{
		return;
	}

	Vector3 gravity(0.0f, 0.0f, GRAVITY_ACCELERATION); // Downward acceleration
	mFallVelocity += gravity * deltaTime;

	if (mFallVelocity.Length() > TERMINAL_VELOCITY)
	{
		mFallVelocity.Normalize();
		mFallVelocity *= TERMINAL_VELOCITY; // Set to terminal velocity
	}

	CollisionComponent* parentCollision = parent->GetComponent<CollisionComponent>();
	if (parentCollision)
	{
		const std::vector<Actor*>& colliders = GetGame()->GetCollider();
		Vector3 totalOffset = Vector3::Zero;

		for (Actor* other : colliders)
		{
			if (other == parent)
			{
				continue;
			}

			CollisionComponent* otherCollision = other->GetComponent<CollisionComponent>();
			if (!otherCollision)
			{
				continue;
			}
			Vector3 offset;
			CollSide side = parentCollision->GetMinOverlap(otherCollision, offset);
			if (side != CollSide::None)
			{
				totalOffset += offset;
			}
			if (side == CollSide::Top && mFallVelocity.z < 0.0f) // Check for landing
			{
				Vector3 pos = parent->GetPosition();
				pos.z -= FALL_POSITION_OFFSET;
				parent->SetPosition(pos);
				Die();
				TurretBase* otherTurretBase = dynamic_cast<TurretBase*>(other);
				if (otherTurretBase)
				{
					float parentHeight = otherCollision->GetMax().z - otherCollision->GetMin().z;
					pos.z -= parentHeight / 2.0f;
					pos.z -= 15.0f;
					parent->SetPosition(pos);
					otherTurretBase->Die();
				}
				return;
			}
		}

		parent->SetPosition(parent->GetPosition() + totalOffset);
	}
}

void TurretHead::UpdateDead(float deltaTime)
{
}

void TurretHead::CheckForTarget()
{
	Actor* lastHit = mLaser->GetLastHitActor();
	if (lastHit)
	{
		HealthComponent* health = lastHit->GetComponent<HealthComponent>();
		if (health && !health->IsDead())
		{
			mAcquiredTarget = lastHit; // Target acquired
			return;
		}
	}
	mAcquiredTarget = nullptr; // No valid target
}

bool TurretHead::CheckForPortalTeleport()
{
	if (mTeleportCooldown > 0.0f)
	{
		return false;
	}

	Portal* bluePortal = GetGame()->GetBluePortal();
	Portal* orangePortal = GetGame()->GetOrangePortal();

	if (!bluePortal || !orangePortal)
	{
		return false;
	}

	CollisionComponent* parentCollision = GetParent()->GetComponent<CollisionComponent>();
	if (!parentCollision)
	{
		return false;
	}

	Portal* entryPortal = nullptr;
	if (parentCollision->Intersect(bluePortal->GetComponent<CollisionComponent>()))
	{
		entryPortal = bluePortal;
	}
	else if (parentCollision->Intersect(orangePortal->GetComponent<CollisionComponent>()))
	{
		entryPortal = orangePortal;
	}

	if (!entryPortal)
	{
		return false;
	}

	Portal* exitPortal = (entryPortal == bluePortal) ? orangePortal : bluePortal;

	GetParent()->SetPosition(exitPortal->GetPosition());
	mFallVelocity += exitPortal->GetQuatForward() * 250.0f;
	mTeleportCooldown = 0.25f;
	return true;
}

void TurretHead::Die()
{
	mTurretState = TurretState::Dead;
	if (Actor* parent = GetParent())
	{
		Quaternion rotation(Vector3::UnitX, Math::PiOver2);
		parent->SetQuat(rotation);
	}

	if (mLaser)
	{
		mLaser->SetEnabled(false);
	}
}
