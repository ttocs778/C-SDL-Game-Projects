#include "CameraComponent.h"
#include "Actor.h"
#include "VehicleMove.h"
#include "Math.h"
#include "Renderer.h"

CameraComponent::CameraComponent(Actor* owner)
: Component(owner)
{
	mDampeningCons = 2.0f * sqrt(mSpringCons);
}

void CameraComponent::Update(float deltaTime)
{

	Vector3 playerPosition = mOwner->GetPosition();
	Vector3 forward = mOwner->GetForward();
	Vector3 idealPosition = CalculateIdeaPos();
	Vector3 displacement = mCamPos - idealPosition;
	Vector3 springAcc = (-mSpringCons * displacement) - (mDampeningCons * mCamVel);
	mCamVel += springAcc * deltaTime;
	mCamPos += mCamVel * deltaTime;
	Vector3 targetPos = playerPosition + (forward * mTargetOffset);
	Matrix4 cameraMatrix = Matrix4::CreateLookAt(mCamPos, targetPos, Vector3::UnitZ);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(cameraMatrix);
}

Vector3 CameraComponent::CalculateIdeaPos()
{
	Vector3 playerPosition = mOwner->GetPosition();
	Vector3 forward = mOwner->GetForward();
	Vector3 idealPosition = playerPosition - (forward * mHDis);
	idealPosition.z = 70.0f;

	return idealPosition;
}

void CameraComponent::SnapToIdeal()
{
	Vector3 idealPosition = CalculateIdeaPos();
	mCamPos = idealPosition;
	mCamVel = Vector3::Zero;
	Vector3 targetPos = mOwner->GetPosition() + (mOwner->GetForward() * mTargetOffset);
	Matrix4 cameraMatrix = Matrix4::CreateLookAt(mCamPos, targetPos, Vector3::UnitZ);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(cameraMatrix);
}