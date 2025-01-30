#include "AnimatedSprite.h"
#include "Actor.h"
#include "Game.h"

AnimatedSprite::AnimatedSprite(Actor* owner, int drawOrder)
: SpriteComponent(owner, drawOrder)
{
}

void AnimatedSprite::Update(float deltaTime)
{
	if (!mAnimName.empty())
	{
		if (!mIsPaused)
		{
			mAnimTimer += mAnimFPS * deltaTime;
		}
		const std::vector<SDL_Texture*>& frames = mAnims[mAnimName];
		size_t numFrames = frames.size();
		while (mAnimTimer >= numFrames)
		{
			mAnimTimer -= numFrames;
		}
		size_t frameIndex = static_cast<size_t>(mAnimTimer);
		SetTexture(frames[frameIndex]);
	}
}

void AnimatedSprite::AddAnimation(const std::string& name, const std::vector<SDL_Texture*>& images)
{
	mAnims.emplace(name, images);
}
