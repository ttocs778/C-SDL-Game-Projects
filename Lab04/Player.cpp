#include "Game.h"
#include "Player.h"
#include "PlayerMove.h"
#include "AnimatedSprite.h"

Player::Player(Game* game)
: Actor(game)
{
	mPlayerAnimation = new AnimatedSprite(this, 200);

	//idle
	std::vector<SDL_Texture*> idleAnim{game->GetTexture("Assets/Mario/Idle.png")};
	mPlayerAnimation->AddAnimation("idle", idleAnim);

	//dead
	std::vector<SDL_Texture*> deadAnim{game->GetTexture("Assets/Mario/Dead.png")};
	mPlayerAnimation->AddAnimation("dead", deadAnim);

	//jumpLeft
	std::vector<SDL_Texture*> jumpLeftAnim{game->GetTexture("Assets/Mario/JumpLeft.png")};
	mPlayerAnimation->AddAnimation("jumpLeft", jumpLeftAnim);

	//jumpRight
	std::vector<SDL_Texture*> jumpRightAnim{game->GetTexture("Assets/Mario/JumpRight.png")};
	mPlayerAnimation->AddAnimation("jumpRight", jumpRightAnim);

	//runleft
	std::vector<SDL_Texture*> runLeftAnim{game->GetTexture("Assets/Mario/RunLeft0.png"),
										  game->GetTexture("Assets/Mario/RunLeft1.png"),
										  game->GetTexture("Assets/Mario/RunLeft2.png")};
	mPlayerAnimation->AddAnimation("runLeft", runLeftAnim);

	//run right
	std::vector<SDL_Texture*> runRightAnim{game->GetTexture("Assets/Mario/RunRight0.png"),
										   game->GetTexture("Assets/Mario/RunRight1.png"),
										   game->GetTexture("Assets/Mario/RunRight2.png")};
	mPlayerAnimation->AddAnimation("runRight", runRightAnim);
	mPlayerAnimation->SetAnimation("idle");
	mPlayerCollision = new CollisionComponent(this);
	mPlayerCollision->SetSize(PLAYER_SIZE, PLAYER_SIZE);
	mPlayerMove = new PlayerMove(this);
}