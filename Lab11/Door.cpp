#include "Door.h"
#include "CollisionComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "MeshComponent.h"

Door::Door(Game* game, const std::string& name)
: Actor(game)
, mName(name)
{
	mDoorCollision = new CollisionComponent(this);
	mDoorMesh = new MeshComponent(this);
	Mesh* doorMesh = game->GetRenderer()->GetMesh("Assets/Meshes/DoorFrame.gpmesh");
	mDoorMesh->SetMesh(doorMesh);
	mDoorCollision->SetSize(doorMesh->GetWidth(), doorMesh->GetHeight(), doorMesh->GetDepth());
	game->AddCollider(this);
	game->AddDoor(name, this);
	mLeftDoor = new Actor(game, this);
	MeshComponent* leftDoorMesh = new MeshComponent(mLeftDoor);
	Mesh* leftMesh = game->GetRenderer()->GetMesh("Assets/Meshes/DoorLeft.gpmesh");
	leftDoorMesh->SetMesh(leftMesh);
	mRightDoor = new Actor(game, this);
	MeshComponent* rightDoorMesh = new MeshComponent(mRightDoor);
	Mesh* rightMesh = game->GetRenderer()->GetMesh("Assets/Meshes/DoorRight.gpmesh");
	rightDoorMesh->SetMesh(rightMesh);
}

Door::~Door()
{
	mGame->RemoveCollider(this);
	mGame->RemoveDoor(mName);
}

void Door::OnUpdate(float deltaTime)
{

	if (mIsOpen)
	{
		mOpenTimer += deltaTime;
		if (mOpenTimer > OPEN_TIME_MAX)
		{
			mOpenTimer = 1.0f;
		}
		Vector3 leftTarget = Vector3(0, -100, 0);
		Vector3 rightTarget = Vector3(0, 100, 0);

		Vector3 leftPosition = Vector3::Lerp(Vector3::Zero, leftTarget, mOpenTimer);
		Vector3 rightPosition = Vector3::Lerp(Vector3::Zero, rightTarget, mOpenTimer);

		mLeftDoor->SetPosition(leftPosition);
		mRightDoor->SetPosition(rightPosition);
	}
}
