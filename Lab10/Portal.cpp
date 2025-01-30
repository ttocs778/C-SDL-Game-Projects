#include "Portal.h"
#include "Game.h"
#include "PortalMeshComponent.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "CollisionComponent.h"

Portal::Portal(class Game* game, bool isBlue)
: Actor(game)
, mIsBlue(isBlue)
{
	mPortalMeshComponent = new PortalMeshComponent(this);
	if (isBlue)
	{
		mPortalMeshComponent->SetTextureIndex(0);
	}
	else
	{
		mPortalMeshComponent->SetTextureIndex(1);
	}
	mMesh = new MeshComponent(this, true);
	mMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/Portal.gpmesh"));
	if (isBlue)
	{
		mMesh->SetTextureIndex(2);
	}
	else
	{
		mMesh->SetTextureIndex(3);
	}
}

Vector3 Portal::GetPortalOutVector(const Vector3& initialVector, Portal* exitPortal, float w)
{
	Matrix4 inverseWT = GetWorldTransform();
	inverseWT.Invert();
	Vector3 transformedVector = Vector3::Transform(initialVector, inverseWT, w);
	Matrix4 yawMatrix = Matrix4::CreateRotationZ(Math::Pi);
	Vector3 flipVector = Vector3::Transform(transformedVector, yawMatrix, w);
	Vector3 exitVector = Vector3::Transform(flipVector, exitPortal->GetWorldTransform(), w);

	return exitVector;
}

void Portal::CalcViewMatrix(struct PortalData& portalData, Portal* exitPortal)
{
	if (exitPortal == nullptr)
	{
		portalData.mView = Matrix4::CreateScale(0.0f);
		return;
	}
	else
	{
		portalData.mCameraPos = GetPortalOutVector(mGame->GetPlayer()->GetPosition(), exitPortal,
												   1.0f);
		portalData.mCameraForward = GetPortalOutVector(
			mGame->GetPlayer()->GetComponent<CameraComponent>()->GetCamForward(), exitPortal, 0.0f);
		portalData.mCameraUp = exitPortal->GetWorldTransform().GetZAxis();
		Vector3 target = portalData.mCameraPos + portalData.mCameraForward * 50.0f;
		portalData.mView = Matrix4::CreateLookAt(portalData.mCameraPos, target,
												 portalData.mCameraUp);
	}
}

void Portal::OnUpdate(float deltaTime)
{

	if (this == GetGame()->GetBluePortal())
	{
		CalcViewMatrix(GetGame()->GetRenderer()->GetBluePortal(), GetGame()->GetOrangePortal());
	}
	else if (this == GetGame()->GetOrangePortal())
	{

		CalcViewMatrix(GetGame()->GetRenderer()->GetOrangePortal(), GetGame()->GetBluePortal());
	}
}

void Portal::SetupCollisionComponent(const Vector3& surfaceNormal)
{
	mPortalCollision = new CollisionComponent(this);
	Vector3 size;
	if (Math::NearlyZero(surfaceNormal.x))
	{
		if (Math::NearlyZero(surfaceNormal.y))
		{
			size = mZ;
		}
		else
		{
			size = mY;
		}
	}
	else
	{
		size = mX;
	}

	mPortalCollision->SetSize(size.x, size.y, size.z);
}