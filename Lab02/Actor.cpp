#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game)
: mGame(game)
, mState(ActorState::Active)
, mPosition(Vector2::Zero)
, mScale(1.0f)
, mRotation(0.0f)
{
	mGame->AddActor(this);
}

Actor::~Actor()
{
	for (auto* component : mComponents)
	{
		delete component;
	}
	mComponents.clear();
	// Bug 1 fixed
	mGame->RemoveActor(this);
}

void Actor::Update(float deltaTime)
{
	if (mState == ActorState::Active)
	{
		for (auto* component : mComponents)
		{
			component->Update(deltaTime);
		}
		OnUpdate(deltaTime);
	}
}

void Actor::OnUpdate(float deltaTime)
{
}

void Actor::ProcessInput(const Uint8* keyState)
{
	if (mState == ActorState::Active)
	{
		for (auto* component : mComponents)
		{
			component->ProcessInput(keyState);
		}
		OnProcessInput(keyState);
	}
}

void Actor::OnProcessInput(const Uint8* keyState)
{
}

void Actor::AddComponent(Component* c)
{
	mComponents.emplace_back(c);
	std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
		return a->GetUpdateOrder() < b->GetUpdateOrder();
	});
}

Vector2 Actor::GetForward() const
{
	float forwardX = Math::Cos(mRotation);
	float forwardY = -Math::Sin(mRotation);
	return Vector2(forwardX, forwardY);
}