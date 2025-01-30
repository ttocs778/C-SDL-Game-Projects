#include "Block.h"
#include "Game.h"

Block::Block(Game* game, char type)
: Actor(game)
{
	mBlockSprite = new SpriteComponent(this);
	mBlockCollision = new CollisionComponent(this);

	// Construct the texture path dynamically
	std::string texturePath = "Assets/Block";
	texturePath += type;   // Append the block type (A, B, C, etc.)
	texturePath += ".png"; // Append the file extension

	// Set the block texture
	mBlockSprite->SetTexture(game->GetTexture(texturePath));

	// Set the collision size
	mBlockCollision->SetSize(BLOCK_SIZE, BLOCK_SIZE);

	// Add the block to the game
	mGame->AddBlock(this);
}

Block::~Block()
{
	mGame->RemoveBlock(this);
}