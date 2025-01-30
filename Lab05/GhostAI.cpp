#include "GhostAI.h"
#include "Actor.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "PathNode.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include "Ghost.h"
#include "PacMan.h"
#include "Random.h"
#include "AnimatedSprite.h"

GhostAI::GhostAI(class Actor* owner)
: Component(owner, 50)
{
	mGhost = static_cast<Ghost*>(owner);
	mGhostAnimation = mGhost->GetComponent<AnimatedSprite>();
}

void GhostAI::Update(float deltaTime)
{
	mStateTime += deltaTime;
	Vector2 ghostPosition = mGhost->GetPosition();

	if (mState == Scatter || mState == Chase)
	{
		ghostPosition += mDirection * deltaTime * SCATTER_CHASE_SPEED;
	}
	else if (mState == Frightened)
	{
		if (mStateTime < 5.0f)
		{
			mGhostAnimation->SetAnimation("scared0");
		}
		else
		{
			mGhostAnimation->SetAnimation("scared1");
		}
		ghostPosition += mDirection * deltaTime * FRIGHTENED_SPEED;
	}
	else if (mState == Dead)
	{
		ghostPosition += mDirection * deltaTime * DEAD_SPEED;
	}

	mGhost->SetPosition(ghostPosition);

	if (mGhost->GetComponent<CollisionComponent>()->Intersect(
			mNextNode->GetComponent<CollisionComponent>()))
	{
		mGhost->SetPosition(mNextNode->GetPosition());
		HandlingStateChanges(deltaTime);
		UpdateTargetNode();
		ChooseNextNode();
		// Combine direction calculation with animation setting here
		CalculateDirection();
	}
}

void GhostAI::Frighten()
{
	if (mState != Dead)
	{
		mStateTime = 0.0f;
		if (mState != Frightened)
		{
			mState = Frightened;
			PathNode* tempNode = mPrevNode;
			mPrevNode = mNextNode;
			mNextNode = tempNode;
			CalculateDirection();
			mTargetNode = nullptr;
		}
	}
}

void GhostAI::Start(PathNode* startNode)
{
	Vector2 pos = startNode->GetPosition();
	mGhost->SetPosition(Vector2(round(pos.x), round(pos.y)));
	mState = Scatter;
	mPrevNode = nullptr;
	mNextNode = startNode;
	mStateTime = 0.0f;
}

void GhostAI::Die()
{
	mStateTime = 0.0f;
	mState = Dead;
	mTargetNode = mGhost->GetGame()->GetGhostPen();
	CalculateDirection();
}

void GhostAI::Chasing()
{
	// Get the ghost type
	Ghost::Type type = mGhost->GetType();

	// Check which ghost we are controlling
	if (type == Ghost::Type::Blinky)
	{
		// BLINKY's logic
		PathNode* pacPrevNode = mGhost->GetGame()->GetPlayer()->GetPrevNode();
		if (pacPrevNode->GetType() == PathNode::Tunnel)
		{
			pacPrevNode = FindClosestDefaultNode(mGhost->GetGame()->GetPlayer()->GetPosition());
		}
		mTargetNode = pacPrevNode;
	}
	else if (type == Ghost::Type::Pinky)
	{
		// PINKY's logic
		Vector2 frontPoints = mGhost->GetGame()->GetPlayer()->GetPointInFrontOf(PINKY_DIS);
		mTargetNode = FindClosestDefaultNode(frontPoints);
	}
	else if (type == Ghost::Type::Inky)
	{
		// INKY's logic
		Vector2 pointP = mGhost->GetGame()->GetPlayer()->GetPointInFrontOf(INKY_DIS);
		std::array<Ghost*, GHOST_COUNT>& ghosts = mOwner->GetGame()->GetGhosts();
		Vector2 blinkyPos = ghosts[0]->GetPosition();
		Vector2 v = pointP - blinkyPos;
		Vector2 vDoubled = 2.0f * v;
		Vector2 pointQ = vDoubled + blinkyPos;
		PathNode* closestNode = FindClosestDefaultNode(pointQ);
		mTargetNode = closestNode;
	}
	else if (type == Ghost::Type::Clyde)
	{
		// CLYDE's logic
		std::array<Ghost*, GHOST_COUNT>& ghosts = mOwner->GetGame()->GetGhosts();
		Ghost* clyde = ghosts[3];
		if (Vector2::Distance(clyde->GetPosition(), mGhost->GetGame()->GetPlayer()->GetPosition()) >
			150.0f)
		{
			mTargetNode = mGhost->GetGame()->GetPlayer()->GetPrevNode();
		}
		else
		{
			mTargetNode = clyde->GetScatterNode();
		}
	}
}

void GhostAI::HandlingStateChanges(float deltaTime)
{
	if (mState == Frightened && mStateTime >= 7.0f)
	{
		mState = Scatter;
		mStateTime = 0.0f;
	}
	if (mNextNode == mOwner->GetGame()->GetGhostPen())
	{
		mState = Scatter;
		mStateTime = 0.0f;
		mTargetNode = mGhost->GetScatterNode();
	}
	if (mState == Scatter && mStateTime >= 5.0f)
	{
		mState = Chase;
		mStateTime = 0.0f;
	}
	else if (mState == Chase && mStateTime >= 20.0f)
	{
		mState = Scatter;
		mStateTime = 0.0f;
	}
}

void GhostAI::UpdateTargetNode()
{
	// Assuming mScatterNode is set as the ghost’s scatter target node
	if (mState == Scatter)
	{
		mTargetNode = mGhost->GetScatterNode();
	}
	if (mState == Frightened)
	{
		std::vector<PathNode*> nodeChoices;
		for (PathNode* adjNode : mNextNode->mAdjacent)
		{
			if (adjNode->GetType() != PathNode::Ghost && adjNode->GetType() != PathNode::Tunnel)
			{
				nodeChoices.emplace_back(adjNode);
			}
		}
		if (nodeChoices.empty())
		{
			for (PathNode* adjNode : mNextNode->mAdjacent)
			{
				if (adjNode != mPrevNode && adjNode->GetType() != PathNode::Tunnel)
				{
					nodeChoices.emplace_back(adjNode);
				}
			}
		}
		if (nodeChoices.empty())
		{
			for (PathNode* adjNode : mNextNode->mAdjacent)
			{
				nodeChoices.emplace_back(adjNode);
			}
		}
		if (!nodeChoices.empty())
		{
			int randNum = Random::GetIntRange(0, static_cast<int>(nodeChoices.size() - 1));
			mTargetNode = nodeChoices[randNum];
		}
	}

	if (mState == Chase)
	{
		Chasing();
	}
}

void GhostAI::ChooseNextNode()
{
	PathNode* bestNode = nullptr;
	float closestDist = FLT_MAX;
	//SDL_Log("Vector2: x = %f, y = %f", mNextNode->GetPosition().x, mNextNode->GetPosition().y);
	for (PathNode* adjNode : mNextNode->mAdjacent)
	{
		if (adjNode != mPrevNode && (mState == Dead || adjNode->GetType() != PathNode::Ghost) &&
			adjNode->GetType() != PathNode::Tunnel)
		{
			float distToTarget = Vector2::Distance(adjNode->GetPosition(),
												   mTargetNode->GetPosition());
			if (distToTarget < closestDist)
			{
				closestDist = distToTarget;
				bestNode = adjNode;
			}
		}
	}

	if (bestNode == nullptr)
	{
		for (PathNode* adjNode : mNextNode->mAdjacent)
		{
			if (adjNode != mPrevNode && adjNode->GetType() != PathNode::Tunnel)
			{
				float distToTarget = Vector2::Distance(adjNode->GetPosition(),
													   mTargetNode->GetPosition());
				if (distToTarget < closestDist)
				{
					closestDist = distToTarget;
					bestNode = adjNode;
				}
			}
		}
	}

	if (bestNode == nullptr)
	{
		for (PathNode* adjNode : mNextNode->mAdjacent)
		{

			float distToTarget = Vector2::Distance(adjNode->GetPosition(),
												   mTargetNode->GetPosition());
			if (distToTarget < closestDist)
			{
				closestDist = distToTarget;
				bestNode = adjNode;
			}
		}
	}
	if (bestNode)
	{
		mPrevNode = mNextNode;
		mNextNode = bestNode;
	}
}

void GhostAI::CalculateDirection()
{
	if (mPrevNode && mNextNode)
	{
		mDirection = mNextNode->GetPosition() - mPrevNode->GetPosition();
		mDirection.Normalize();

		// Set the animation based on the calculated direction
		if (mState == Scatter || mState == Chase)
		{
			if (mDirection.x < 0)
			{
				mGhostAnimation->SetAnimation("left");
			}
			else if (mDirection.x > 0)
			{
				mGhostAnimation->SetAnimation("right");
			}
			else if (mDirection.y > 0)
			{
				mGhostAnimation->SetAnimation("down");
			}
			else if (mDirection.y < 0)
			{
				mGhostAnimation->SetAnimation("up");
			}
		}
		else if (mState == Dead)
		{
			if (mDirection.x < 0)
			{
				mGhostAnimation->SetAnimation("deadleft");
			}
			else if (mDirection.x > 0)
			{
				mGhostAnimation->SetAnimation("deadright");
			}
			else if (mDirection.y > 0)
			{
				mGhostAnimation->SetAnimation("deaddown");
			}
			else if (mDirection.y < 0)
			{
				mGhostAnimation->SetAnimation("deadup");
			}
		}
	}
}

PathNode* GhostAI::FindClosestDefaultNode(const Vector2& pacManPos)
{
	PathNode* closestNode = nullptr;
	float closestDist = FLT_MAX;

	// Loop through all path nodes in the game
	for (PathNode* node : mGhost->GetGame()->GetPathNodes())
	{
		if (node->GetType() == PathNode::Default)
		{
			float dist = Vector2::Distance(node->GetPosition(), pacManPos);
			if (dist < closestDist)
			{
				closestDist = dist;
				closestNode = node;
			}
		}
	}
	return closestNode;
}

void GhostAI::DebugDrawPath(SDL_Renderer* render)
{
	// Draw a rectangle at the target node
	if (mTargetNode != nullptr)
	{
		const int SIZE = 16;
		SDL_Rect r;
		r.x = static_cast<int>(mTargetNode->GetPosition().x) - SIZE / 2;
		r.y = static_cast<int>(mTargetNode->GetPosition().y) - SIZE / 2;
		r.w = SIZE;
		r.h = SIZE;
		SDL_RenderDrawRect(render, &r);
	}

	// Line from ghost to next node
	if (mNextNode != nullptr)
	{
		SDL_RenderDrawLine(render, static_cast<int>(mOwner->GetPosition().x),
						   static_cast<int>(mOwner->GetPosition().y),
						   static_cast<int>(mNextNode->GetPosition().x),
						   static_cast<int>(mNextNode->GetPosition().y));
	}
}