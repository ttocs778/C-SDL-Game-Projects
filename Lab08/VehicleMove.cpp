#include "VehicleMove.h"
#include "Actor.h"
#include "HeightMap.h"
#include "Game.h"
#include "fstream"
#include "CSVHelper.h"

VehicleMove::VehicleMove(Actor* owner)
: Component(owner, 50)
{
	std::ifstream file("Assets/HeightMap/Checkpoints.csv");
	if (!file.is_open())
	{
		SDL_Log("Failed to load Assets/HeightMap/Checkpoints.csv");
		return;
	}
	std::string line;
	std::getline(file, line);
	while (std::getline(file, line))
	{
		if (!line.empty())
		{
			std::vector<std::string> strings = CSVHelper::Split(line);
			int minCellX = std::stoi(strings[1]);
			int maxCellX = std::stoi(strings[2]);
			int minCellY = std::stoi(strings[3]);
			int maxCellY = std::stoi(strings[4]);
			std::vector<int> checkPoint = {minCellX, minCellY, maxCellX, maxCellY};
			mCheckPoints.push_back(checkPoint);
		}
	}
	file.close();
}

void VehicleMove::Update(float deltaTime)
{
	UpdateAcceleration(deltaTime);
	UpdatePosition(deltaTime);
	UpdateRotation(deltaTime);
	CheckCheckpoint();
}

void VehicleMove::UpdateAcceleration(float deltaTime)
{
	if (mIsPedalPress)
	{
		mAccDuration += deltaTime;
		float t = Math::Min(mAccDuration / mAccRampTime, 1.0f);
		float accMagnitude = Math::Lerp(MIN_LIN_ACC, MAX_LIN_ACC, t);
		mVelocity += mOwner->GetForward() * accMagnitude * deltaTime;
	}
	else
	{
		mAccDuration = 0.0f;
	}

	// Apply drag based on whether pedal is pressed
	if (mIsPedalPress)
	{
		mVelocity *= mLinDrag;
	}
	else
	{
		mVelocity *= mLinDragNotPress;
	}
}

void VehicleMove::UpdatePosition(float deltaTime)
{
	Vector3 position = mOwner->GetPosition();
	Vector3 newPosition = position + (mVelocity * deltaTime);
	HeightMap* heightMap = GetGame()->GetHeightMap();

	if (heightMap->IsOnTrack(Vector2{newPosition.x, newPosition.y}))
	{
		newPosition.z = Math::Lerp(
			newPosition.z, heightMap->GetHeight(Vector2{newPosition.x, newPosition.y}), 0.1f);
	}

	mOwner->SetPosition(newPosition);
}

void VehicleMove::UpdateRotation(float deltaTime)
{
	// Determine turning direction
	if (mTurn == VehicleTurn::Left)
		mTurnFactor = -1.0f;
	else if (mTurn == VehicleTurn::Right)
		mTurnFactor = 1.0f;
	else if (mTurn == VehicleTurn::None)
		mTurnFactor = 0.0f;

	// Update angular velocity and rotation
	mAngularVelocity += mAngularAcc * mTurnFactor * deltaTime;
	float angle = mOwner->GetRotation() + (mAngularVelocity * deltaTime);
	mOwner->SetRotation(angle);
	mAngularVelocity *= mAngularDrag;
}

void VehicleMove::CheckCheckpoint()
{
	Vector2 cellPos = GetGame()->GetHeightMap()->WorldToCell(
		Vector2{mOwner->GetPosition().x, mOwner->GetPosition().y});
	int cellX = static_cast<int>(cellPos.x);
	int cellY = static_cast<int>(cellPos.y);

	for (size_t i = 0; i < mCheckPoints.size(); ++i)
	{
		auto& checkPoint = mCheckPoints[i];
		if (cellX >= checkPoint[0] && cellX <= checkPoint[2] && cellY >= checkPoint[1] &&
			cellY <= checkPoint[3])
		{
			if (static_cast<int>(i) == (mLastCheckPoint + 1) % mCheckPoints.size())
			{
				mLastCheckPoint = static_cast<int>(i);
				if (mLastCheckPoint == 0)
				{
					mCurrentLap++;
					OnLapChange(mCurrentLap);
				}
			}
		}
	}
}
