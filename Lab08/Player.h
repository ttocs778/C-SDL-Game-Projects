#pragma once
#include "Actor.h"
#include "PlayerUI.h"

class Player : public Actor
{
public:
	Player(Game* game);
	void OnUpdate(float deltaTime) override;

private:
	class MeshComponent* mMc;
	class PlayerMove* mPlayerMove;
	class CameraComponent* mCamComponent;
	class PlayerUI* mPlayerUi;
};