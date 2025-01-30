#pragma once
#include "Component.h"
#include "Math.h"
#include <vector>

class AnimatedSprite;
class GhostAI : public Component
{
public:
	// Used to track the four different GhostAI states
	enum State
	{
		Scatter,
		Chase,
		Frightened,
		Dead
	};

	GhostAI(class Actor* owner);

	void Update(float deltaTime) override;

	// Called when the Ghost starts at the beginning
	// (or when the ghosts should respawn)
	void Start(class PathNode* startNode);

	// Get the current state
	State GetState() const { return mState; }

	// Called when the ghost should switch to the "Frightened" state
	void Frighten();

	// Called when the ghost should switch to the "Dead" state
	void Die();

	void UpdateTargetNode();

	void ChooseNextNode();

	//  Helper function to draw GhostAI's current goal, prev, and next
	void DebugDrawPath(struct SDL_Renderer* render);

	void CalculateDirection();

	void HandlingStateChanges(float deltaTime);

	PathNode* FindClosestDefaultNode(const Vector2& pacManPos);

	void Chasing();

private:
	// Member data for pathfinding

	// TargetNode is our current goal node
	class PathNode* mTargetNode = nullptr;
	// PrevNode is the last node we intersected
	// with prior to the current position
	class PathNode* mPrevNode = nullptr;
	// NextNode is the next node we're trying to get to
	class PathNode* mNextNode = nullptr;

	// Current state of the Ghost AI
	State mState = Scatter;

	// Save the owning actor (cast to a Ghost*)
	class Ghost* mGhost;

	Vector2 mDirection{0.0f, 0.0f};

	const float SCATTER_CHASE_SPEED = 90.0f;

	const float FRIGHTENED_SPEED = 65.0f;

	const float DEAD_SPEED = 125.0f;

	const float PINKY_DIS = 80.0f;

	const float INKY_DIS = 40.0f;

	float mStateTime = 0.0;

	AnimatedSprite* mGhostAnimation;

	static const int GHOST_COUNT = 4;
	// TODO: Add any member data/helper functions here!
};
