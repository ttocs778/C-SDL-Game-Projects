#pragma once
#include "Component.h"
#include "Game.h"

class CameraComponent : public Component
{
public:
	CameraComponent(class Actor* owner);
	void Update(float deltaTime) override;
	Vector3 CalculateIdeaPos();
	void SnapToIdeal();
	float GetPitchSpeed() const { return mPitchSpeed; }
	void SetPitchSpeed(float pitchSpeed) { mPitchSpeed = pitchSpeed; }
	float GetPitchAngle() const { return mPitchAngle; }
	const Vector3& GetCamForward() const { return mCamForward; }
	void ResetPitch() { mPitchAngle = 0.0f; }

private:
	const float TARGET_OFFSET = 50.0f;
	float mPitchAngle = 0.0f;
	float mPitchSpeed = 0.0f;
	Vector3 mCamForward;
};