#include "Game.h"
#include "Component.h"
#include <algorithm>
#include "Actor.h"

Actor::Actor(class Game* game, Actor* parent)
: mGame(game)
, mState(ActorState::Active)
, mPosition(Vector3::Zero)
, mScale(Vector3(1.0f, 1.0f, 1.0f))
, mRotation(0.0f)
, mRollRotation(0.0f)
, mParent(parent)
{
	if (mParent == nullptr)
	{
		mGame->AddActor(this);
	}
	else
	{
		mParent->AddChild(this);
	}
}

Actor::~Actor()
{

	GetGame()->GetAudio()->RemoveActor(this);
	while (!mChildren.empty())
	{
		Actor* child = mChildren.back();
		mChildren.pop_back();
		child->mParent = nullptr; // Detach child from parent
		delete child;
	}
	if (mParent == nullptr)
	{
		mGame->RemoveActor(this);
	}
	else
	{
		mParent->RemoveChild(this);
	}

	for (auto* component : mComponents)
	{
		delete component;
	}
	mComponents.clear();
}

void Actor::Update(float deltaTime)
{
	CalcWorldTransform();
	if (mState == ActorState::Active)
	{
		for (auto* component : mComponents)
		{
			component->Update(deltaTime);
		}
		OnUpdate(deltaTime);
	}
	CalcWorldTransform();
	for (auto* child : mChildren)
	{
		child->Update(deltaTime);
	}
}

void Actor::OnUpdate(float deltaTime)
{
}

void Actor::ProcessInput(const Uint8* keyState, Uint32 mouseButtons, const Vector2& relativeMouse)
{
	if (mState == ActorState::Active)
	{
		for (auto* component : mComponents)
		{
			component->ProcessInput(keyState, mouseButtons, relativeMouse);
		}
		OnProcessInput(keyState, mouseButtons, relativeMouse);
	}
}

void Actor::OnProcessInput(const Uint8* keyState, Uint32 mouseButtons, const Vector2& relativeMouse)
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
	Matrix4 quatMat = Matrix4::CreateFromQuaternion(mQuat);
	Matrix4 transMat = Matrix4::CreateTranslation(mPosition);
	Matrix4 rotMatX = Matrix4::CreateRotationX(mRollRotation);

	mWorldTransform = scaleMat * rotMat * quatMat * rotMatX * transMat;
	if (mParent)
	{
		if (mInheritScale)
		{
			mWorldTransform *= mParent->GetWorldTransform();
		}
		else
		{
			mWorldTransform *= mParent->GetWorldRotTrans();
		}
	}
}

Vector3 Actor::GetQuatForward() const
{
	Vector3 fwd = Vector3::Transform(Vector3::UnitX, mQuat);
	fwd.Normalize();

	return fwd;
}

Vector3 Actor::GetRight() const
{
	float forwardX = Math::Cos(mRotation + Math::PiOver2);
	float forwardY = Math::Sin(mRotation + Math::PiOver2);
	float forwardZ = 0.0;
	return Vector3(forwardX, forwardY, forwardZ);
}

Matrix4 Actor::GetWorldRotTrans() const
{
	Matrix4 rotMat = Matrix4::CreateRotationZ(mRotation);
	Matrix4 quatMat = Matrix4::CreateFromQuaternion(mQuat);
	Matrix4 transMat = Matrix4::CreateTranslation(mPosition);
	Matrix4 rotMatX = Matrix4::CreateRotationX(mRollRotation);

	Matrix4 localRotTrans = rotMat * quatMat * rotMatX * transMat;
	if (mParent)
	{
		localRotTrans *= mParent->GetWorldRotTrans();
	}
	return localRotTrans;
}

Vector3 Actor::GetWorldPosition() const
{
	return mWorldTransform.GetTranslation();
}

Vector3 Actor::GetWorldForward() const
{
	return mWorldTransform.GetXAxis();
}

void Actor::AddChild(Actor* child)
{
	mChildren.emplace_back(child);
}

void Actor::RemoveChild(Actor* child)
{
	auto iter = std::find(mChildren.begin(), mChildren.end(), child);
	if (iter != mChildren.end())
	{
		mChildren.erase(iter);
	}
}