#include "Effect.h"
#include "AnimatedSprite.h"

Effect::Effect(Game* game, const Vector2& effectPos, const std::string& animName,
			   const std::string& soundName)
: Actor(game)
{
	SetPosition(effectPos);
	AnimatedSprite* effectAnim = new AnimatedSprite(this, 200);
	effectAnim->LoadAnimations("Assets/Effects");
	effectAnim->SetAnimation(animName);
	mLifeTime = effectAnim->GetAnimDuration(animName);
	game->GetAudio()->PlaySound(soundName);
}

void Effect::OnUpdate(float deltaTime)
{
	mLifeTime -= deltaTime;
	if (mLifeTime <= 0.0f)
	{
		SetState(ActorState::Destroy);
	}
}