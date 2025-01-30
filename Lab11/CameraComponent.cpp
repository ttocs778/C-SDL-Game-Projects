#include "CameraComponent.h"
#include "Actor.h"
#include "Math.h"
#include "Renderer.h"

CameraComponent::CameraComponent(Actor* owner)
: Component(owner)
{
}

void CameraComponent::Update(float deltaTime)
{
	mPitchAngle += mPitchSpeed * deltaTime;
	mPitchAngle = Math::Clamp(mPitchAngle, -Math::Pi / 2.1f, Math::Pi / 2.1f);
	Matrix4 yawMatrix = Matrix4::CreateRotationZ(mOwner->GetRotation());
	Matrix4 pitchMatricx = Matrix4::CreateRotationY(mPitchAngle);
	Matrix4 combinedMatrix = pitchMatricx * yawMatrix;
	mCamForward = Vector3::Transform(Vector3::UnitX, combinedMatrix);
	Vector3 playerPosition = mOwner->GetPosition();
	Vector3 eye = playerPosition;
	Vector3 targetPos = eye + mCamForward * TARGET_OFFSET;
	Matrix4 cameraMatrix = Matrix4::CreateLookAt(eye, targetPos, Vector3::UnitZ);
	mOwner->GetGame()->GetRenderer()->SetViewMatrix(cameraMatrix);
}
