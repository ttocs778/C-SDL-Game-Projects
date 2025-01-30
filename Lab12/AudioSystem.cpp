#include "AudioSystem.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <filesystem>
#include "Game.h"
#include "Actor.h"

SoundHandle SoundHandle::Invalid;

// Create the AudioSystem with specified number of channels
// (Defaults to 8 channels)
AudioSystem::AudioSystem(Game* game, int numChannels)
: mGame(game) // Initialize the Game pointer
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_AllocateChannels(numChannels);
	mChannels.resize(numChannels);
}

// Destroy the AudioSystem
AudioSystem::~AudioSystem()
{
	for (auto& sound : mSounds)
	{
		Mix_FreeChunk(sound.second);
	}
	mSounds.clear();
	Mix_CloseAudio();
}

// Updates the status of all the active sounds every frame
void AudioSystem::Update(float deltaTime)
{
	for (int i = 0; i < mChannels.size(); ++i)
	{
		SoundHandle& handle = mChannels[i];
		if (handle.IsValid())
		{
			auto iter = mHandleMap.find(handle);
			if (iter != mHandleMap.end())
			{
				HandleInfo& info = iter->second;

				// Adjust volume if the sound has an associated actor
				if (info.mActor)
				{
					Actor* player = mGame->GetPlayer();
					int volume = CalculateVolume(info.mActor, player);
					Mix_Volume(info.mChannel, volume);
				}

				// Check if the sound has stopped
				if (Mix_Playing(info.mChannel) == 0)
				{
					// Clean up the handle if the sound stopped
					mHandleMap.erase(handle);
					if (info.mActor)
					{
						mActorMap[info.mActor].erase(handle);
						if (mActorMap[info.mActor].empty())
						{
							mActorMap.erase(info.mActor);
						}
					}
					handle.Reset();
				}
			}
		}
	}
}

// Plays the sound with the specified name and loops if looping is true
// Returns the SoundHandle which is used to perform any other actions on the
// sound when active
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
SoundHandle AudioSystem::PlaySound(const std::string& soundName, bool looping, Actor* actor,
								   bool stopOnActorRemove, int fadeTimeMS)
{
	Mix_Chunk* chunk = GetSound(soundName);
	if (!chunk)
	{
		SDL_Log("[AudioSystem] PlaySound couldn't find sound for %s", soundName.c_str());
		return SoundHandle::Invalid;
	}

	int channelToUse = -1;
	for (int i = 0; i < mChannels.size(); ++i)
	{
		if (!mChannels[i].IsValid())
		{
			channelToUse = i;
			break;
		}
	}

	if (channelToUse == -1)
	{
		SDL_Log("[AudioSystem] No available channel for %s", soundName.c_str());
		return SoundHandle::Invalid;
	}

	++mLastHandle;

	HandleInfo info;
	info.mSoundName = soundName;
	info.mChannel = channelToUse;
	info.mIsLooping = looping;
	info.mActor = actor;
	info.mStopOnActorRemove = stopOnActorRemove;

	mHandleMap.emplace(mLastHandle, info);
	mChannels[channelToUse] = mLastHandle;

	if (actor)
	{
		mActorMap[actor].insert(mLastHandle); // Add sound handle to the actor map
	}

	if (fadeTimeMS > 0)
	{
		Mix_FadeInChannel(channelToUse, chunk, looping ? -1 : 0, fadeTimeMS);
	}
	else
	{
		Mix_PlayChannel(channelToUse, chunk, looping ? -1 : 0);
	}

	Actor* player = mGame->GetPlayer();
	int volume = CalculateVolume(actor, player);
	Mix_Volume(channelToUse, volume);

	return mLastHandle;
}

// Stops the sound if it is currently playing
void AudioSystem::StopSound(SoundHandle sound, int fadeTimeMS)
{
	auto iter = mHandleMap.find(sound);
	if (iter == mHandleMap.end())
	{
		SDL_Log("[AudioSystem] StopSound couldn't find handle %s", sound.GetDebugStr());
		return;
	}

	HandleInfo& info = iter->second;
	if (fadeTimeMS > 0)
	{
		Mix_FadeOutChannel(info.mChannel, fadeTimeMS);
	}
	else
	{
		Mix_HaltChannel(info.mChannel);
		mChannels[info.mChannel].Reset();
		mHandleMap.erase(iter);
	}
}

// Pauses the sound if it is currently playing
void AudioSystem::PauseSound(SoundHandle sound)
{
	auto iter = mHandleMap.find(sound);
	if (iter == mHandleMap.end())
	{
		SDL_Log("[AudioSystem] PauseSound couldn't find handle %s", sound.GetDebugStr());
		return;
	}

	HandleInfo& info = iter->second;

	if (info.mIsPaused)
	{
		SDL_Log("[AudioSystem] Sound %s is already paused", sound.GetDebugStr());
		return;
	}

	Mix_Pause(info.mChannel);

	info.mIsPaused = true;
}

// Resumes the sound if it is currently paused
void AudioSystem::ResumeSound(SoundHandle sound)
{
	auto iter = mHandleMap.find(sound);
	if (iter == mHandleMap.end())
	{
		SDL_Log("[AudioSystem] ResumeSound couldn't find handle %s", sound.GetDebugStr());
		return;
	}

	HandleInfo& info = iter->second;

	if (!info.mIsPaused)
	{
		SDL_Log("[AudioSystem] Sound %s is already playing", sound.GetDebugStr());
		return;
	}

	Mix_Resume(info.mChannel);

	info.mIsPaused = false;
}

// Returns the current state of the sound
SoundState AudioSystem::GetSoundState(SoundHandle sound)
{
	auto iter = mHandleMap.find(sound);
	if (iter == mHandleMap.end())
	{
		return SoundState::Stopped;
	}
	const HandleInfo& info = iter->second;
	if (info.mIsPaused)
	{
		return SoundState::Paused;
	}
	return SoundState::Playing;
}

// Stops all sounds on all channels
void AudioSystem::StopAllSounds()
{
	Mix_HaltChannel(-1);
	for (auto& channel : mChannels)
	{
		channel.Reset();
	}
	mHandleMap.clear();
}

// Cache all sounds under Assets/Sounds
void AudioSystem::CacheAllSounds()
{
#ifndef __clang_analyzer__
	std::error_code ec{};
	for (const auto& rootDirEntry : std::filesystem::directory_iterator{"Assets/Sounds", ec})
	{
		std::string extension = rootDirEntry.path().extension().string();
		if (extension == ".ogg" || extension == ".wav")
		{
			std::string fileName = rootDirEntry.path().stem().string();
			fileName += extension;
			CacheSound(fileName);
		}
	}
#endif
}

// Used to preload the sound data of a sound
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
void AudioSystem::CacheSound(const std::string& soundName)
{
	GetSound(soundName);
}

// If the sound is already loaded, returns Mix_Chunk from the map.
// Otherwise, will attempt to load the file and save it in the map.
// Returns nullptr if sound is not found.
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
Mix_Chunk* AudioSystem::GetSound(const std::string& soundName)
{
	std::string fileName = "Assets/Sounds/";
	fileName += soundName;

	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	else
	{
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk)
		{
			SDL_Log("[AudioSystem] Failed to load sound file %s", fileName.c_str());
			return nullptr;
		}

		mSounds.emplace(fileName, chunk);
	}
	return chunk;
}

// Input for debugging purposes
void AudioSystem::ProcessInput(const Uint8* keyState)
{
	// Debugging code that outputs all active sounds on leading edge of period key
	if (keyState[SDL_SCANCODE_PERIOD] && !mLastDebugKey)
	{
		SDL_Log("[AudioSystem] Active Sounds:");
		for (size_t i = 0; i < mChannels.size(); i++)
		{
			if (mChannels[i].IsValid())
			{
				auto iter = mHandleMap.find(mChannels[i]);
				if (iter != mHandleMap.end())
				{
					HandleInfo& hi = iter->second;
					SDL_Log("Channel %d: %s, %s, looping = %d, paused = %d",
							static_cast<unsigned>(i), mChannels[i].GetDebugStr(),
							hi.mSoundName.c_str(), hi.mIsLooping, hi.mIsPaused);
				}
				else
				{
					SDL_Log("Channel %d: %s INVALID", static_cast<unsigned>(i),
							mChannels[i].GetDebugStr());
				}
			}
		}
	}

	mLastDebugKey = keyState[SDL_SCANCODE_PERIOD];
}

void AudioSystem::RemoveActor(Actor* actor)
{
	// Check if the actor is in the map
	auto actorIter = mActorMap.find(actor);
	if (actorIter != mActorMap.end())
	{
		// Iterate over all sound handles for this actor
		for (const SoundHandle& handle : actorIter->second)
		{
			auto handleIter = mHandleMap.find(handle);
			if (handleIter != mHandleMap.end())
			{
				HandleInfo& info = handleIter->second;
				// Set mActor to nullptr
				info.mActor = nullptr;

				// Stop sound if mStopOnActorRemove is true and the sound is playing
				if (info.mStopOnActorRemove && Mix_Playing(info.mChannel))
				{
					Mix_HaltChannel(info.mChannel);
				}
			}
		}
		// Remove the actor from the map
		mActorMap.erase(actor);
	}
}

int AudioSystem::CalculateVolume(class Actor* actor, class Actor* listener) const
{
	// Return max volume if either actor or listener is null
	if (!actor || !listener)
	{
		return 128;
	}

	// Get the world positions of the actor and listener
	Vector3 actorPosition = actor->GetWorldPosition();
	Vector3 listenerPosition = listener->GetWorldPosition();

	// Calculate the distance between the actor and listener
	float distance = Vector3::Distance(actorPosition, listenerPosition);

	// Define the inner and outer radii
	const float innerRadius = 25.0f;
	const float outerRadius = 600.0f;

	// Handle the volume based on the distance
	if (distance <= innerRadius)
	{
		return 128; // Max volume
	}
	else if (distance >= outerRadius)
	{
		return 0; // Min volume
	}
	else
	{
		// Linear interpolation between inner and outer radius
		float t = (distance - innerRadius) / (outerRadius - innerRadius);
		return static_cast<int>((1.0f - t) * 128);
	}
}
