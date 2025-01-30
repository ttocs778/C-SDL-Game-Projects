#include "Pellet.h"
#include "Mesh.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Portal.h"
#include "EnergyCatcher.h"
#include "EnergyCube.h"
#include "EnergyGlass.h"
#include "HealthComponent.h"

Pellet::Pellet(Game* game)
: Actor(game)
{
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Meshes/Sphere.gpmesh");
	mPelletMesh = new MeshComponent(this);
	mPelletMesh->SetMesh(mesh);
	mPelletMesh->SetTextureIndex(1);
	mPelletCollision = new CollisionComponent(this);
	mPelletCollision->SetSize(PELLET_COLL_SIZE, PELLET_COLL_SIZE, PELLET_COLL_SIZE);
}

void Pellet::OnUpdate(float deltaTime)
{
	Vector3 newPosition = GetPosition() + mVelocity * deltaTime;
	SetPosition(newPosition);
	mLifeTime += deltaTime;
	if (mLifeTime < LIFE_TIME_CAP && mTeleportedRecently)
		return;
	if (mPelletCollision->Intersect(mGame->GetPlayer()->GetComponent<CollisionComponent>()))
	{
		HealthComponent* playerHealth = mGame->GetPlayer()->GetComponent<HealthComponent>();
		if (playerHealth)
		{
			playerHealth->TakeDamage(100.0f, GetPosition());
		}
		SetState(ActorState::Destroy);
		return;
	}

	Portal* bluePortal = GetGame()->GetBluePortal();
	Portal* orangePortal = GetGame()->GetOrangePortal();
	if (bluePortal && orangePortal)
	{
		if (mPelletCollision->Intersect(bluePortal->GetComponent<CollisionComponent>()))
		{
			CalPelletTeleport(bluePortal, orangePortal);
			mTeleportedRecently = true;
			mLifeTime = 0.0f;
			return;
		}
		else if (mPelletCollision->Intersect(orangePortal->GetComponent<CollisionComponent>()))
		{
			CalPelletTeleport(orangePortal, bluePortal);
			mTeleportedRecently = true;
			mLifeTime = 0.0f;
			return;
		}
	}
	if (mLifeTime >= LIFE_TIME_CAP)
	{
		for (auto collider : mGame->GetCollider())
		{
			if (!collider || collider == this)
				continue;
			EnergyCatcher* energyCatcher = dynamic_cast<EnergyCatcher*>(collider);
			if (energyCatcher &&
				mPelletCollision->Intersect(energyCatcher->GetComponent<CollisionComponent>()) &&
				!energyCatcher->GetIsActivated())
			{
				SetPosition(energyCatcher->GetPosition() + energyCatcher->GetForward() * 40.0f);
				mVelocity = Vector3::Zero;
				energyCatcher->SetIsActivated(true);
				return;
			}
			EnergyCube* energyCube = dynamic_cast<EnergyCube*>(collider);
			if (energyCube &&
				mPelletCollision->Intersect(energyCube->GetComponent<CollisionComponent>()))
			{
				mPelletMesh->SetTextureIndex(2);
				mIsGreen = true;
				return;
			}
			EnergyGlass* energyGlass = dynamic_cast<EnergyGlass*>(collider);
			if (energyGlass &&
				mPelletCollision->Intersect(energyGlass->GetComponent<CollisionComponent>()))
			{
				if (mIsGreen)
				{
					return;
				}
				else
				{
					SetState(ActorState::Destroy);
				}
			}
			if (mPelletCollision->Intersect(collider->GetComponent<CollisionComponent>()))
			{
				HealthComponent* health = collider->GetComponent<HealthComponent>();
				if (health && !health->IsDead())
				{
					health->TakeDamage(100.0f, GetPosition());
					SetState(ActorState::Destroy);
				}
				else
				{
					SetState(ActorState::Destroy);
				}
				return;
			}
		}
	}
}

void Pellet::CalPelletTeleport(Portal* entry, Portal* exit)
{
	Vector3 teleportVel = entry->GetPortalOutVector(Vector3::Normalize(mVelocity), exit, 0.0f);
	teleportVel.Normalize();
	mVelocity = teleportVel * 500.0f;
	Vector3 teleportPos = entry->GetPortalOutVector(GetPosition(), exit, 1.0f);
	SetPosition(teleportPos);
}