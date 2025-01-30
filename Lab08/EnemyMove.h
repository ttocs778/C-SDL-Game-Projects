#pragma once
#include "VehicleMove.h"
#include <string>
#include <vector>

class EnemyMove : public VehicleMove
{
public:
	EnemyMove(class Actor* owner, const std::string& fileName);
	void Update(float deltaTime) override;

private:
	std::vector<Vector3> mNodes;
	int mNextNode = 1;
	const float THRESHOLD = 53.0f;
	const float ALIGNMENT_THRESHOLD = 0.995f;
	const float FORWARD_THRESHOLD = 0.85f;
};
