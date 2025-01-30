#pragma once
#include "MeshComponent.h"
#include "Actor.h"
#include "SegmentCast.h"
#include <vector>

class LaserComponent : public MeshComponent
{
private:
	std::vector<LineSegment> mLineSegments;
	Actor* mIgnoreActor = nullptr;
	Actor* mLastHitActor = nullptr;
	bool mEnabled = true;
	const float LASER_DISTANCE = 350.0f;

public:
	LaserComponent(Actor* owner);
	void Update(float deltaTime) override;
	void Draw(class Shader* shader) override;
	Matrix4 CalWorldTransform(LineSegment segment);
	void SetIgnoreActor(Actor* actor) { mIgnoreActor = actor; }
	Actor* GetLastHitActor() const { return mLastHitActor; }
	void SetEnabled(bool enabled) { mEnabled = enabled; }
	bool IsEnabled() const { return mEnabled; }
};
