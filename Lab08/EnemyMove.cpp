#include "EnemyMove.h"
#include "Actor.h"
#include "Game.h"
#include <fstream>
#include "CSVHelper.h"
#include "HeightMap.h"
#include "Math.h"

EnemyMove::EnemyMove(Actor* owner, const std::string& fileName)
: VehicleMove(owner)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("Failed to load CSV: %s", fileName.c_str());
		return;
	}
	std::string line;
	std::getline(file, line);
	while (std::getline(file, line))
	{
		if (!line.empty())
		{
			std::vector<std::string> strings = CSVHelper::Split(line);
			int cellX = std::stoi(strings[1]);
			int cellY = std::stoi(strings[2]);
			Vector3 worldPos = GetGame()->GetHeightMap()->CellToWorld(cellX, cellY);
			mNodes.push_back(worldPos);
		}
	}
	file.close();
	if (!mNodes.empty())
	{
		owner->SetPosition(mNodes[0]);
	}
}

void EnemyMove::Update(float deltaTime)
{
	//update next node
	Vector3 enemyPosition = mOwner->GetPosition();
	Vector3 nodePostition = mNodes[mNextNode];

	float disToNextNode = Vector3::Distance(nodePostition, enemyPosition);
	if (disToNextNode < THRESHOLD)
	{
		mNextNode++;
		if (mNextNode >= mNodes.size())
		{
			mNextNode = 0;
		}
	}
	Vector3 directionToNext = Vector3::Normalize(nodePostition - enemyPosition);
	Vector3 enemyForward = mOwner->GetForward();
	float dotProduct = Vector3::Dot(directionToNext, enemyForward);

	SetPedal(dotProduct > FORWARD_THRESHOLD);

	if (dotProduct < ALIGNMENT_THRESHOLD)
	{
		Vector3 crossProduct = Vector3::Cross(directionToNext, enemyForward);
		if (crossProduct.z > 0.0f)
		{
			SetTurn(VehicleTurn::Left);
		}
		else
		{
			SetTurn(VehicleTurn::Right);
		}
	}
	else
	{
		SetTurn(VehicleTurn::None);
	}

	VehicleMove::Update(deltaTime);
}
