#include "Player.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "PlayerMove.h"
#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "PlayerMesh.h"
#include "HealthComponent.h"
#include "HUD.h"
#include "Random.h"

Player::Player(Game* game)
: Actor(game)
{
	mPlayerMove = new PlayerMove(this);
	mCamComponent = new CameraComponent(this);
	mPlayerCollision = new CollisionComponent(this);
	mPlayerCollision->SetSize(50.0f, 100.0f, 50.0f);
	mPlayerHealth = new HealthComponent(this, 100.0f);

	if (mPlayerHealth)
	{
		// Set the OnDamage callback
		mPlayerHealth->SetOnDamageCallback([this](const Vector3& location) {
			HandlePlayerDamage(location);
		});

		mPlayerHealth->SetOnDeathCallback([this] {
			HandlePlayerDeath();
		});
	}

	mHUD = new HUD(this);
}

void Player::OnUpdate(float deltaTime)
{
}

void Player::GiveGun()
{
	mHasGun = true;
	mPlayerMesh = new PlayerMesh(GetGame());
}

void Player::HandlePlayerDamage(const Vector3& location)
{
	// Get vector from player to the damage location
	Vector3 damageDir = location - GetWorldPosition();
	damageDir.z = 0.0f; // Set Z component to 0
	damageDir.Normalize();

	// Get the forward vector of the player
	Vector3 forward = GetForward();
	forward.z = 0.0f;
	forward.Normalize();

	float angle = Math::Acos(Vector3::Dot(forward, damageDir));
	// Use the cross product to determine if the angle is positive or negative
	Vector3 cross = Vector3::Cross(forward, damageDir);
	if (cross.z > 0.0f)
	{
		angle = -angle;
	}

	// Call PlayerTakeDamage on HUD
	mHUD->PlayerTakeDamage(angle);
}

void Player::HandlePlayerDeath()
{
	std::vector<std::pair<std::string, std::string>> taunts = {
		{"Glados-PlayerDead1.ogg", "Congratulations! The test is now over."},
		{"Glados-PlayerDead2.ogg",
		 "Thank you for participating in this Aperture Science computer-aided enrichment activity."},
		{"Glados-PlayerDead3.ogg", "Goodbye."},
		{"Glados-PlayerDead4.ogg", "You're not a good person. You know that, right?"}};

	int index = Random::GetIntRange(0, taunts.size() - 1);
	const auto& [sound, subtitle] = taunts[index];

	mDeathSound = GetGame()->GetAudio()->PlaySound(sound, false, this);
	mHUD->ShowSubtitle(subtitle);
	mHUD->SetDamageOverlay(true);

	SDL_Log("Player has died. Playing taunt: %s", subtitle.c_str());
}