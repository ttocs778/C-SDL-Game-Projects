#include "Mesh.h"
#include "Game.h"
#include "PortalGun.h"
#include "CollisionComponent.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Player.h"
#include "CameraComponent.h"
#include "PlayerMesh.h"

PlayerMesh::PlayerMesh(Game* game)
: Actor(game)
{
	mPlayerMesh = new MeshComponent(this);
	mPlayerMesh->SetMesh(game->GetRenderer()->GetMesh("Assets/Meshes/PortalGun.gpmesh"));
	SetScale(Vector3(1.0f, 2.5f, 2.5f));
}

void PlayerMesh::OnUpdate(float deltaTime)
{
	Vector3 vector = mGame->GetRenderer()->Unproject(Vector3(300.0f, -250.0f, 0.4f));
	SetPosition(vector);
	float pitchAngle = mGame->GetPlayer()->GetComponent<CameraComponent>()->GetPitchAngle();
	Quaternion pitchRotation = Quaternion(Vector3::UnitY, pitchAngle);
	float yawAngle = mGame->GetPlayer()->GetRotation();
	Quaternion yawRotation = Quaternion(Vector3::UnitZ, yawAngle);
	Quaternion rotation = Quaternion::Concatenate(pitchRotation, yawRotation);
	SetQuat(rotation);
}