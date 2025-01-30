#pragma once
#include "Texture.h"

#include "UIComponent.h"
#include <string>
class Font;

class HUD : public UIComponent
{
public:
	HUD(class Actor* owner);
	~HUD();

	// Override the Draw function
	void Draw(class Shader* shader) override;
	void Update(float deltaTime) override;
	void ShowSubtitle(const std::string& text);
	void PlayerTakeDamage(float angle);
	void SetDamageOverlay(bool active);

private:
	class Font* mFont;
	Texture* mSubtitle = nullptr;
	Texture* mSubtitleShadow = nullptr;
	Texture* mDamageIndicatorTexture;
	float mDamageIndicatorAngle = 0.0f;
	float mDamageIndicatorTime = 0.0f;
	Texture* mDamageOverlayTexture;
	bool mDamageOverlayActive = false;
};
