#include "PlayerMove.h"
#include "Actor.h"
#include "Math.h"
#include "Renderer.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "HeightMap.h"
#include "Enemy.h"

PlayerMove::PlayerMove(Actor* owner)
: VehicleMove(owner)
{
	mOwner->SetPosition(GetGame()->GetHeightMap()->CellToWorld(39, 58));
}

void PlayerMove::Update(float deltaTime)
{
	VehicleMove::Update(deltaTime);
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	if (keyState[SDL_SCANCODE_W])
	{
		SetPedal(true);
	}
	else
	{
		SetPedal(false);
	}
	if (keyState[SDL_SCANCODE_A] && !keyState[SDL_SCANCODE_D])
	{
		SetTurn(VehicleTurn::Left);
	}
	else if (keyState[SDL_SCANCODE_D] && !keyState[SDL_SCANCODE_A])
	{
		SetTurn(VehicleTurn::Right);
	}
	else
	{
		SetTurn(VehicleTurn::None);
	}
}

void PlayerMove::OnLapChange(int newLap)
{
	auto audioSystem = mOwner->GetGame()->GetAudio();
	auto playerUI = mOwner->GetComponent<PlayerUI>();
	playerUI->OnLapChange(newLap);

	if (newLap == 2)
	{
		audioSystem->StopSound(mOwner->GetGame()->GetMusicHandle(), 250);

		audioSystem->PlaySound("FinalLap.wav", false);
		SoundHandle fastMusicHandle = audioSystem->PlaySound("MusicFast.ogg", true, 4000);
		mOwner->GetGame()->SetMusicHandle(fastMusicHandle);
	}
	else if (newLap >= 3)
	{
		int enemyLap = mOwner->GetGame()->GetEnemy()->GetComponent<EnemyMove>()->GetCurrentLap();
		bool playerWon = (newLap > enemyLap);

		playerUI->SetRaceState(playerWon ? PlayerUI::Won : PlayerUI::Lost);
		mOwner->SetState(ActorState::Paused);
		mOwner->GetGame()->GetEnemy()->SetState(ActorState::Paused);

		audioSystem->StopSound(mOwner->GetGame()->GetMusicHandle(), 250);

		if (playerWon)
		{
			audioSystem->PlaySound("Won.wav", false);
		}
		else
		{
			audioSystem->PlaySound("Lost.wav", false);
		}
	}
	else
	{
		playerUI->OnLapChange(newLap);
	}
}
