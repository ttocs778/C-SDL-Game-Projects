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

private:
	float mHDis = 60.0f;
	float mTargetOffset = 50.0f;
	float mSpringCons = 256.0f;
	float mDampeningCons;
	Vector3 mCamPos;
	Vector3 mCamVel;
};