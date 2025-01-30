#include "LaserComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "SegmentCast.h"

LaserComponent::LaserComponent(Actor* owner)
: MeshComponent(owner)
{
	SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Meshes/Laser.gpmesh"));
}

void LaserComponent::Update(float deltaTime)
{
	if (!mEnabled)
	{
		mLineSegments.clear();
		return;
	}
	mLineSegments.clear();
	mLastHitActor = nullptr;

	Vector3 startPoint = mOwner->GetWorldPosition();
	Vector3 direction = mOwner->GetWorldForward();
	Vector3 endPoint = startPoint + mOwner->GetWorldForward() * LASER_DISTANCE;
	LineSegment segment(startPoint, endPoint);

	CastInfo collisionInfo;
	const std::vector<Actor*>& actors = GetGame()->GetActor();

	if (SegmentCast(actors, segment, collisionInfo, mIgnoreActor))
	{
		endPoint = collisionInfo.mPoint;
		mLastHitActor = collisionInfo.mActor;
	}
	mLineSegments.emplace_back(LineSegment(startPoint, endPoint));
	// Check for portal collision
	Portal* bluePortal = GetGame()->GetBluePortal();
	Portal* orangePortal = GetGame()->GetOrangePortal();

	if (bluePortal && orangePortal && collisionInfo.mActor)
	{
		Portal* entryPortal = dynamic_cast<Portal*>(collisionInfo.mActor);
		Portal* exitPortal = (entryPortal == bluePortal) ? orangePortal : bluePortal;

		if (entryPortal && exitPortal)
		{
			Vector3 transformedDirection = entryPortal->GetPortalOutVector(direction, exitPortal,
																		   0.0f);
			transformedDirection.Normalize();
			Vector3 transformedStart = entryPortal->GetPortalOutVector(collisionInfo.mPoint,
																	   exitPortal, 1.0f);
			transformedStart += transformedDirection * 5.5f;

			Vector3 transformedEnd = transformedStart + transformedDirection * 350.0f;

			LineSegment secondSegment(transformedStart, transformedEnd);

			CastInfo secondCollisionInfo;
			if (SegmentCast(actors, secondSegment, secondCollisionInfo, exitPortal))
			{
				transformedEnd = secondCollisionInfo.mPoint;
				mLastHitActor = secondCollisionInfo.mActor;
			}
			else
			{
				mLastHitActor = nullptr;
			}

			mLineSegments.emplace_back(LineSegment(transformedStart, transformedEnd));
		}
	}
	if (mLastHitActor && mLastHitActor->GetComponent<Portal>())
	{
		mLastHitActor = nullptr;
	}
	if (!SegmentCast(actors, segment, collisionInfo, mIgnoreActor))
	{
		mLastHitActor = nullptr;
	}
}

void LaserComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
		for (const LineSegment& segment : mLineSegments)
		{
			Matrix4 transform = CalWorldTransform(segment);
			shader->SetMatrixUniform("uWorldTransform", transform);
			Texture* t = mMesh->GetTexture(mTextureIndex);
			if (t)
			{
				t->SetActive();
			}
			VertexArray* va = mMesh->GetVertexArray();
			va->SetActive();
			glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
		}
	}
}

Matrix4 LaserComponent::CalWorldTransform(LineSegment segment)
{
	float length = segment.Length();
	Matrix4 scale = Matrix4::CreateScale(length, 1.0f, 1.0f);
	Vector3 direction = segment.mEnd - segment.mStart;
	direction.Normalize();
	Vector3 defaultForward = Vector3::UnitX;
	float dot = Vector3::Dot(defaultForward, direction);
	Quaternion rotation;

	if (dot == 1.0f)
	{
		rotation = Quaternion::Identity;
	}
	else if (dot == -1.0f)
	{
		rotation = Quaternion(Vector3::UnitY, Math::Pi);
	}
	else
	{
		// Calculate the quaternion to rotate from defaultForward to direction
		Vector3 axis = Vector3::Cross(defaultForward, direction);
		axis.Normalize();
		float angle = Math::Acos(dot);
		rotation = Quaternion(axis, angle);
	}

	Matrix4 rotationMatrix = Matrix4::CreateFromQuaternion(rotation);

	Vector3 centerPoint = segment.PointOnSegment(0.5f);
	Matrix4 translation = Matrix4::CreateTranslation(centerPoint);
	return scale * rotationMatrix * translation;
}
