#pragma once
#include <vector>
#include "Engine/Renderer/AnimTypes.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class IsoSpriteDefenition;
class SpriteDefenition;
class SpriteSheet;
struct Vec3;

//------------------------------------------------------------------------------------------------------------------------------
class IsoAnimDefenition
{
public:
	IsoAnimDefenition(const SpriteSheet& sheet, int startDefIndex, int endDefIndex,
		float durationSeconds, const std::string& animName, const std::vector<IsoSpriteDefenition>& isoSpriteDefs, SpriteAnimPlaybackType playbackType = SPRITE_ANIM_PLAYBACK_LOOP);

	inline float				GetAnimDuration() { return m_durationSeconds; }
	void						SetIsoSpriteDefenitions(const std::vector<IsoSpriteDefenition>& isoSpriteDefs);
	void						AddIsoSpriteDefenition(const IsoSpriteDefenition& isoSpriteDef);

	IsoSpriteDefenition&		GetIsoSpriteAtTime(float seconds);

private:
	IsoSpriteDefenition&		GetSpriteDefAtTime_Once(float seconds);
	IsoSpriteDefenition&		GetSpriteDefAtTime_Loop(float seconds);
	IsoSpriteDefenition&		GetSpriteDefAtTime_PingPong(float seconds);

private:
	const SpriteSheet&					m_spriteSheet;
	std::vector<IsoSpriteDefenition>	m_isoSpriteDefs;
	std::string							m_animName;	//Basically our Animation ID so to speak
	int									m_startDefIndex = -1;
	int									m_endDefIndex = -1;
	float								m_durationSeconds = 1.f;
	SpriteAnimPlaybackType				m_playbackType = SPRITE_ANIM_PLAYBACK_LOOP;
};