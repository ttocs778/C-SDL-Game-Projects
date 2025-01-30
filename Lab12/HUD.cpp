#include "HUD.h"
#include "Shader.h"
#include "Font.h"
#include "Texture.h"
#include "Game.h"
#include "Renderer.h"

HUD::HUD(Actor* owner)
: UIComponent(owner)
{
	mFont = new Font();
	if (!mFont->Load("Assets/Inconsolata-Regular.ttf"))
	{
		SDL_Log("Failed to load font: Assets/Inconsolata-Regular.ttf");
	}
	mDamageIndicatorTexture =
		mOwner->GetGame()->GetRenderer()->GetTexture("Assets/Textures/UI/DamageIndicator.png");
	mDamageOverlayTexture =
		mOwner->GetGame()->GetRenderer()->GetTexture("Assets/Textures/UI/DamageOverlay.png");
}

HUD::~HUD()
{
	if (mFont)
	{
		mFont->Unload();
		delete mFont;
		mFont = nullptr;
	}
}

void HUD::Update(float deltaTime)
{
	// Decrease the damage indicator timer
	if (mDamageIndicatorTime > 0.0f)
	{
		mDamageIndicatorTime -= deltaTime;
	}
}

void HUD::Draw(Shader* shader)
{
	if (mDamageIndicatorTime > 0.0f)
	{

		DrawTexture(shader, mDamageIndicatorTexture, Vector2::Zero, 1.0, mDamageIndicatorAngle);
	}

	if (mSubtitle)
	{
		// Calculate the screen position for the subtitle
		float height = static_cast<float>(mSubtitle->GetHeight());
		Vector2 subtitlePosition(0.0f, -325.0f + height / 2.0f);

		if (mSubtitleShadow)
		{
			Vector2 shadowPosition = subtitlePosition + Vector2(2.0f, -2.0f);
			DrawTexture(shader, mSubtitleShadow, shadowPosition);
		}
		DrawTexture(shader, mSubtitle, subtitlePosition);
	}
}

void HUD::ShowSubtitle(const std::string& text)
{
	// Unload existing textures if they exist
	if (mSubtitle)
	{
		mSubtitle->Unload();
		delete mSubtitle;
		mSubtitle = nullptr;
	}

	if (mSubtitleShadow)
	{
		mSubtitleShadow->Unload();
		delete mSubtitleShadow;
		mSubtitleShadow = nullptr;
	}

	// If the text is empty, clear subtitles
	if (text.empty())
	{
		return;
	}

	// Prepend "GLaDOS: " to the text
	std::string fullText = "GLaDOS: " + text;

	// Render the subtitle and shadow textures
	mSubtitle = mFont->RenderText(fullText, Color::LightGreen);
	mSubtitleShadow = mFont->RenderText(fullText, Color::Black);
}

void HUD::PlayerTakeDamage(float angle)
{
	mDamageIndicatorAngle = angle;
	mDamageIndicatorTime = 1.5f; // Set the time to 1.5 seconds
}

void HUD::SetDamageOverlay(bool active)
{
	mDamageOverlayActive = active;
}