#include "PlayerUI.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "Actor.h"
#include "VehicleMove.h"
#include "Enemy.h"
#include "HeightMap.h"

PlayerUI::PlayerUI(Actor* owner)
: UIComponent(owner)
{
	Renderer* r = owner->GetGame()->GetRenderer();
	mLapTextures = std::vector<Texture*>{
		r->GetTexture("Assets/UI/Lap1.png"),
		r->GetTexture("Assets/UI/FinalLap.png"),
	};

	mFirstTexture = r->GetTexture("Assets/UI/1st.png");
	mSecondTexture = r->GetTexture("Assets/UI/2nd.png");

	mGoTexture = r->GetTexture("Assets/UI/Go.png");
	mReadyTexture = r->GetTexture("Assets/UI/Ready.png");
}

void PlayerUI::Update(float deltaTime)
{
	mGoDisplayTimer -= deltaTime;
	mLapDisplayTimer -= deltaTime;
}

void PlayerUI::Draw(Shader* shader)
{
	if (mGoDisplayTimer > 0.0f)
	{
		if (mOwner->GetState() == ActorState::Paused)
		{
			DrawTexture(shader, mReadyTexture, Vector2(0.0f, 100.0f));
		}
		else if (mOwner->GetState() == ActorState::Active)
		{
			DrawTexture(shader, mGoTexture, Vector2(0.0f, 100.0f));
		}
	}

	if (mLapDisplayTimer > 0.0f)
	{
		DrawTexture(shader, mLapTextures[mLapIndex], Vector2(0.0f, 200.0f), 0.75f);
	}

	if (mRaceState == Won)
	{
		DrawTexture(shader, mFirstTexture, Vector2(0.0f, 100.0f));
	}
	else if (mRaceState == Lost)
	{
		DrawTexture(shader, mSecondTexture, Vector2(0.0f, 100.0f));
	}

	// Figure out what place to show
	if (mOwner->GetState() == ActorState::Active)
	{
		bool inFirst = IsPlayerInFirst();
		if (inFirst)
		{
			DrawTexture(shader, mFirstTexture, Vector2(400.0f, 320.0f), 0.5f);
		}
		else
		{
			DrawTexture(shader, mSecondTexture, Vector2(400.0f, 320.0f), 0.5f);
		}
	}
}

void PlayerUI::OnLapChange(int lapNum)
{
	if (lapNum >= 1 && lapNum <= mLapTextures.size())
	{
		mLapIndex = lapNum - 1;
		mLapDisplayTimer = 3.0f;
	}
}

bool PlayerUI::IsPlayerInFirst() const
{
	const VehicleMove* playerMove = mOwner->GetComponent<VehicleMove>();
	const VehicleMove* enemyMove = mOwner->GetGame()->GetEnemy()->GetComponent<VehicleMove>();
	const HeightMap* heightMap = mOwner->GetGame()->GetHeightMap();

	// Step 1: Compare lap numbers
	if (playerMove->GetCurrentLap() > enemyMove->GetCurrentLap())
	{
		return true;
	}
	else if (playerMove->GetCurrentLap() < enemyMove->GetCurrentLap())
	{
		return false;
	}

	// Step 2: Compare checkpoint numbers
	if (playerMove->GetLastCheckPoint() > enemyMove->GetLastCheckPoint())
	{
		return true;
	}
	else if (playerMove->GetLastCheckPoint() < enemyMove->GetLastCheckPoint())
	{
		return false;
	}

	// Step 3: Same lap and checkpoint, so check distance to the next checkpoint
	Vector3 playerPosition = mOwner->GetPosition();
	Vector3 enemyPosition = mOwner->GetGame()->GetEnemy()->GetPosition();
	const auto& playerNextCheckpointCells =
		playerMove->GetCheckPoints()[(playerMove->GetLastCheckPoint() + 1) %
									 playerMove->GetCheckPoints().size()];
	const auto& enemyNextCheckpointCells =
		enemyMove->GetCheckPoints()[(enemyMove->GetLastCheckPoint() + 1) %
									enemyMove->GetCheckPoints().size()];

	Vector3 playerNextCheckpointWorld = heightMap->CellToWorld(playerNextCheckpointCells[0],
															   playerNextCheckpointCells[1]);
	Vector3 enemyNextCheckpointWorld = heightMap->CellToWorld(enemyNextCheckpointCells[0],
															  enemyNextCheckpointCells[1]);

	// Calculate distances to the next checkpoint for both player and enemy
	float playerDistance = Vector3::Distance(playerPosition, playerNextCheckpointWorld);
	float enemyDistance = Vector3::Distance(enemyPosition, enemyNextCheckpointWorld);
	return playerDistance < enemyDistance;
}
