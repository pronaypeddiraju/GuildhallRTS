#pragma once
#include <map>
#include "Engine/Renderer/IsoSpriteDefenition.hpp"
#include "Engine/Renderer/AnimTypes.hpp"

class IsoAnimDefenition;

class Animator
{
public:
	void Update(float deltaTime);
	IsoAnimDefenition* GetCurrentFrame() const;

	void Play(IsoAnimDefenition *defenition, SpriteAnimPlaybackType mode);
	void Play(std::string const &name, SpriteAnimPlaybackType mode);

private:
	float ApplyLoop3Mode(float elapsedTime, float animationTime, SpriteAnimPlaybackType mode);

private:

	std::map<std::string, IsoAnimDefenition*>	m_animSet;
	IsoAnimDefenition*							m_currentAnimation = nullptr;
	float									m_totalElapsedTime = 0.f;
	SpriteAnimPlaybackType					m_loopMode = SPRITE_ANIM_PLAYBACK_ONCE;
};