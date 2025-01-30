#pragma once

#include "Actor.h"
#include <string>
#include <vector>
#include "AudioSystem.h"

class CollisionComponent;

class VOTrigger : public Actor
{
public:
	VOTrigger(class Game* game);

	void OnUpdate(float deltaTime) override;
	void OnProcessInput(const Uint8* keyState, Uint32 mouseButtons,
						const Vector2& relativeMouse) override;

	// Setters for JSON properties
	void SetDoorName(const std::string& doorName) { mDoorName = doorName; }
	void SetNextLevel(const std::string& nextLevel) { mNextLevel = nextLevel; }
	void SetSounds(const std::vector<std::string>& sounds) { mSounds = sounds; }
	void SetSubtitles(const std::vector<std::string>& subtitles) { mSubtitles = subtitles; }

private:
	CollisionComponent* mCollision;

	// Additional properties
	std::string mDoorName;
	std::string mNextLevel;
	std::vector<std::string> mSounds;
	std::vector<std::string> mSubtitles;
	bool mIsActive = false;
	size_t mCurrentSoundIndex = 0;
	SoundHandle mCurrentSound;

	void Activate();	  // Trigger activation logic
	void PlayNextSound(); // Helper function to play the next sound
	void Finish();
};
