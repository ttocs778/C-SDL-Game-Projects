#include "VOTrigger.h"
#include "Game.h"
#include "AudioSystem.h"
#include "CollisionComponent.h"
#include "Door.h"
#include "HUD.h"
#include "HealthComponent.h"

VOTrigger::VOTrigger(Game* game)
: Actor(game)
, mCollision(new CollisionComponent(this))
{
	mCollision->SetSize(1.0f, 1.0f, 1.0f);
}

void VOTrigger::OnUpdate(float deltaTime)
{
	Player* player = mGame->GetPlayer();
	if (!player)
		return;

	HealthComponent* health = player->GetComponent<HealthComponent>();
	if (health && health->IsDead())
	{
		if (mCurrentSound.IsValid())
		{
			mGame->GetAudio()->StopSound(mCurrentSound);
		}
		return;
	}
	if (!mIsActive)
	{
		// Check if the player intersects with this trigger
		Actor* player = GetGame()->GetPlayer();
		if (player && mCollision->Intersect(player->GetComponent<CollisionComponent>()))
		{
			Activate();
		}
	}
	else
	{
		// Check if the current sound has stopped
		if (mCurrentSound.IsValid() &&
			GetGame()->GetAudio()->GetSoundState(mCurrentSound) == SoundState::Stopped)
		{
			PlayNextSound();
		}
	}
}

void VOTrigger::OnProcessInput(const Uint8* keyState, Uint32 mouseButtons,
							   const Vector2& relativeMouse)
{
	static bool wasFPressed = false;
	bool isFPressed = keyState[SDL_SCANCODE_F] != 0;

	if (isFPressed && !wasFPressed)
	{
		SDL_Log("VOTrigger: Fast Forward triggered");
		if (mCurrentSound.IsValid())
		{
			GetGame()->GetAudio()->StopSound(mCurrentSound);
			PlayNextSound();
		}
	}

	wasFPressed = isFPressed;
}

void VOTrigger::Activate()
{
	mIsActive = true;
	mCurrentSoundIndex = 0;
	PlayNextSound();
}

void VOTrigger::PlayNextSound()
{
	// Check if all sounds have been played
	if (mCurrentSoundIndex >= mSounds.size())
	{
		// Clear subtitle when finished
		if (GetGame()->GetPlayer())
		{
			GetGame()->GetPlayer()->GetHUD()->ShowSubtitle("");
		}

		Finish();
		return;
	}

	// Play the current sound
	const std::string& soundName = mSounds[mCurrentSoundIndex];
	mCurrentSound = GetGame()->GetAudio()->PlaySound(soundName);
	SDL_Log("VOTrigger: Playing sound '%s'", soundName.c_str());

	// Show the corresponding subtitle
	if (GetGame()->GetPlayer())
	{
		GetGame()->GetPlayer()->GetHUD()->ShowSubtitle(mSubtitles[mCurrentSoundIndex]);
	}

	// Increment to the next sound
	mCurrentSoundIndex++;
}

void VOTrigger::Finish()
{
	// Open door if specified
	if (!mDoorName.empty())
	{
		Door* door = GetGame()->GetDoor(mDoorName);
		if (door)
		{
			mGame->RemoveCollider(door);
			door->SetIsOpen(true);
		}
	}

	// Load the next level if specified
	if (!mNextLevel.empty())
	{
		GetGame()->SetNextLevel(mNextLevel);
	}

	// Destroy this VOTrigger
	SetState(ActorState::Destroy);
}
