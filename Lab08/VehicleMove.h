#pragma once
#include "Component.h"
#include "Math.h"
#include "Actor.h"
#include <vector>

class VehicleMove : public Component
{
public:
	VehicleMove(class Actor* owner);
	void Update(float deltaTime) override;
	enum VehicleTurn
	{
		Left,
		Right,
		None
	};
	void SetPedal(bool isPedalPress) { mIsPedalPress = isPedalPress; }
	void SetTurn(VehicleTurn turn) { mTurn = turn; }
	virtual void OnLapChange(int newLao) {}
	int GetLastCheckPoint() const { return mLastCheckPoint; }
	int GetCurrentLap() const { return mCurrentLap; }
	const std::vector<std::vector<int>>& GetCheckPoints() const { return mCheckPoints; };
	void UpdateAcceleration(float deltaTime);
	void UpdatePosition(float deltaTime);
	void UpdateRotation(float deltaTime);
	void CheckCheckpoint();

private:
	bool mIsPedalPress = false;
	VehicleTurn mTurn = VehicleTurn::None;
	Vector3 mVelocity;
	float mAngularVelocity = 0.0f;
	float mAccDuration = 0.0f;
	const float MIN_LIN_ACC = 1000.0f;
	const float MAX_LIN_ACC = 2500.0f;
	float mAccRampTime = 2.0f;
	float mAngularAcc = 5.0f * Math::Pi;
	float mLinDrag = 0.9f;
	float mLinDragNotPress = 0.975f;
	float mAngularDrag = 0.9f;
	float mTurnFactor = 0.0;
	std::vector<std::vector<int>> mCheckPoints;
	int mCurrentLap = 0;
	int mLastCheckPoint = -1;
};