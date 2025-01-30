#include "Game.h"
#include "Component.h"
#include <algorithm>
#include "Actor.h"

Actor::Actor(Game* game)
: mGame(game)
, mState(ActorState::Active)
, mPosition(Vector3::Zero)
, mScale(Vector3(1.0f, 1.0f, 1.0f))
, mRotation(0.0f)
, mRollRotation(0.0f)
{
	mGame->AddActor(this);
}

Actor::~Actor()
{
	mGame->RemoveActor(this);
	for (auto* component : mComponents)
	{
		delete component;
	}
	mComponents.clear();
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
	CalcWorldTransform();
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

Vector3 Actor::GetForward() const
{
	float forwardX = Math::Cos(mRotation);
	float forwardY = Math::Sin(mRotation);
	float forwardZ = 0.0;
	return Vector3(forwardX, forwardY, forwardZ);
}

void Actor::CalcWorldTransform()
{
	Matrix4 scaleMat = Matrix4::CreateScale(mScale);
	Matrix4 rotMat = Matrix4::CreateRotationZ(mRotation);
	Matrix4 transMat = Matrix4::CreateTranslation(mPosition);
	Matrix4 rotMatX = Matrix4::CreateRotationX(mRollRotation);

	mWorldTransform = scaleMat * rotMat * rotMatX * transMat;
}
