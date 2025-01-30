#include "AudioSystem.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <filesystem>

SoundHandle SoundHandle::Invalid;

// Create the AudioSystem with specified number of channels
// (Defaults to 8 channels)
AudioSystem::AudioSystem(int numChannels)
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
			if (Mix_Playing(i) == 0)
			{
				mHandleMap.erase(handle);
				handle.Reset();
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
SoundHandle AudioSystem::PlaySound(const std::string& soundName, bool looping, int fadeTimeMS)
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

	// If no channel is available, apply prioritization
	if (channelToUse == -1)
	{
		SoundHandle oldestSameSound;
		SoundHandle oldestNonLooping;
		SoundHandle oldest;
		for (const auto& entry : mHandleMap)
		{
			const SoundHandle& handle = entry.first;
			const HandleInfo& info = entry.second;

			if (info.mSoundName == soundName &&
				(oldestSameSound == SoundHandle::Invalid || handle < oldestSameSound))
			{
				oldestSameSound = handle;
			}

			if (!info.mIsLooping &&
				(oldestNonLooping == SoundHandle::Invalid || handle < oldestNonLooping))
			{
				oldestNonLooping = handle;
			}

			if (oldest == SoundHandle::Invalid || handle < oldest)
			{
				oldest = handle;
			}
		}

		// Prioritize based on the rules
		SoundHandle handleToOverwrite;
		if (oldestSameSound.IsValid())
		{
			handleToOverwrite = oldestSameSound;
		}
		else if (oldestNonLooping.IsValid())
		{
			handleToOverwrite = oldestNonLooping;
		}
		else
		{
			handleToOverwrite = oldest;
		}

		// Find the channel to overwrite
		auto iter = mHandleMap.find(handleToOverwrite);
		if (iter != mHandleMap.end())
		{
			channelToUse = iter->second.mChannel;
			SDL_Log("[AudioSystem] PlaySound ran out of channels playing %s! Stopping %s",
					soundName.c_str(), iter->second.mSoundName.c_str());

			Mix_HaltChannel(channelToUse);
			mChannels[channelToUse].Reset();
			mHandleMap.erase(iter);
		}
	}

	++mLastHandle;

	HandleInfo info;
	info.mSoundName = soundName;
	info.mIsLooping = looping;
	info.mIsPaused = false;
	info.mChannel = channelToUse;
	mHandleMap.emplace(mLastHandle, info);
	mChannels[channelToUse] = mLastHandle;

	// Play with or without fade-in
	if (fadeTimeMS > 0)
	{
		Mix_FadeInChannel(channelToUse, chunk, looping ? -1 : 0, fadeTimeMS);
	}
	else
	{
		Mix_PlayChannel(channelToUse, chunk, looping ? -1 : 0);
	}

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
