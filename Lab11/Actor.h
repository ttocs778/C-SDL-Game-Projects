#pragma once
#include <vector>
#include <SDL2/SDL_stdinc.h>
#include "Math.h"

enum class ActorState
{
	Active,
	Paused,
	Destroy
};

class Actor
{
public:
	Actor(class Game* game, Actor* parent = nullptr);
	virtual ~Actor();

	// Update function called from Game (not overridable)
	void Update(float deltaTime);
	// ProcessInput function called from Game (not overridable)
	void ProcessInput(const Uint8* keyState, Uint32 mouseButtons, const Vector2& relativeMouse);

	// Getters/setters
	const Vector3& GetPosition() const { return mPosition; }
	void SetPosition(const Vector3& pos) { mPosition = pos; }
	const Vector3& GetScale() const { return mScale; }
	void SetScale(float scale)
	{
		mScale.x = scale;
		mScale.y = scale;
		mScale.z = scale;
	}
	void SetScale(const Vector3& scale) { mScale = scale; }

	float GetRotation() const { return mRotation; }
	void SetRotation(float rotation) { mRotation = rotation; }
	void SetRollRotation(float rotation) { mRollRotation = rotation; }
	float GetRollRotation() const { return mRollRotation; }

	ActorState GetState() const { return mState; }
	void SetState(ActorState state) { mState = state; }

	class Game* GetGame() { return mGame; }

	const Matrix4& GetWorldTransform() const { return mWorldTransform; }

	// Returns component of type T, or null if doesn't exist
	template <typename T>
	T* GetComponent() const
	{
		for (auto c : mComponents)
		{
			T* t = dynamic_cast<T*>(c);
			if (t != nullptr)
			{
				return t;
			}
		}

		return nullptr;
	}
	Vector3 GetForward() const;
	void CalcWorldTransform();
	void SetQuat(const Quaternion& quat) { mQuat = quat; }
	const Quaternion& GetQuat() const { return mQuat; }
	Vector3 GetQuatForward() const;
	Vector3 GetRight() const;
	Matrix4 GetWorldRotTrans() const;
	Vector3 GetWorldPosition() const;
	Vector3 GetWorldForward() const;
	Actor* GetParent() const { return mParent; }
	void AddChild(Actor* child);
	void RemoveChild(Actor* child);

protected:
	// Any actor-specific update code (overridable)
	virtual void OnUpdate(float deltaTime);
	// Any actor-specific update code (overridable)
	virtual void OnProcessInput(const Uint8* keyState, Uint32 mouseButtons,
								const Vector2& relativeMouse);

	class Game* mGame;
	// Actor's state
	ActorState mState;

	// Transform
	Vector3 mPosition;
	Vector3 mScale;
	float mRotation;
	float mRollRotation;

	// Components
	std::vector<class Component*> mComponents;

	Matrix4 mWorldTransform;

	Actor* mParent;

	std::vector<Actor*> mChildren;

	bool mInheritScale = false;

private:
	friend class Component;
	// Adds component to Actor (this is automatically called
	// in the component constructor)
	void AddComponent(class Component* c);
	Quaternion mQuat;
};
