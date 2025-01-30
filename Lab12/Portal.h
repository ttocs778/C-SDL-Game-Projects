#pragma once
#include "Actor.h"

class Portal : public Actor
{
public:
	Portal(class Game* game, bool isBlue);
	Vector3 GetPortalOutVector(const Vector3& initialVector, Portal* exitPortal, float w);
	void OnUpdate(float deltaTime) override;
	void SetupCollisionComponent(const Vector3& surfaceNormal);

private:
	class PortalMeshComponent* mPortalMeshComponent;
	class MeshComponent* mMesh;
	class CollisionComponent* mPortalCollision = nullptr;
	void CalcViewMatrix(struct PortalData& portalData, Portal* exitPortal);
	bool mIsBlue = false;
	Vector3 mZ{110.0f, 10.0f, 125.0f};
	Vector3 mY{10.0f, 125.0f, 110.0f};
	Vector3 mX{110.0f, 125.0f, 10.0f};
};