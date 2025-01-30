#include "Goomba.h"
#include "Game.h"
#include "GoombaMove.h"
#include "AnimatedSprite.h"

Goomba ::Goomba(Game* game)
: Actor(game)
{
	mGoombaSprite = new SpriteComponent(this, 150);
	mGoombaCollision = new CollisionComponent(this);
	mGoombaCollision->SetSize(GOOMBA_SIZE, GOOMBA_SIZE);
	mGame->AddGoomba(this);
	mGoombaMove = new GoombaMove(this);
	mGoombaMove->SetForwardSpeed(-100.0f);
	mGoombaAnimation = new AnimatedSprite(this);
	std::vector<SDL_Texture*> walkAnim{GetGame()->GetTexture("Assets/Goomba/Walk0.png"),
									   GetGame()->GetTexture("Assets/Goomba/Walk1.png")};
	mGoombaAnimation->AddAnimation("walk", walkAnim);
	mGoombaAnimation->SetAnimation("walk");
	std::vector<SDL_Texture*> deadAnim{GetGame()->GetTexture("Assets/Goomba/Dead.png")};
	mGoombaAnimation->AddAnimation("dead", deadAnim);
}

Goomba ::~Goomba()
{
	mGame->RemoveGoomba(this);
}
