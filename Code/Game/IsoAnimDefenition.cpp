#include "Game/IsoAnimDefenition.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/IsoSpriteDefenition.hpp"

//------------------------------------------------------------------------------------------------------------------------------
IsoAnimDefenition::IsoAnimDefenition(const SpriteSheet& sheet, int startDefIndex, int endDefIndex, float durationSeconds, const std::string& animName , SpriteAnimPlaybackType playbackType /*= SPRITE_ANIM_PLAYBACK_LOOP*/)
	: m_spriteSheet(sheet)
{
	m_startDefIndex = startDefIndex;
	m_endDefIndex = endDefIndex;
	m_durationSeconds = durationSeconds;
	m_playbackType = playbackType;
	m_animName = animName;
}

//------------------------------------------------------------------------------------------------------------------------------
void IsoAnimDefenition::SetIsoSpriteDefenitions(const std::vector<IsoSpriteDefenition>& isoSpriteDefs)
{
	m_isoSpriteDefs.clear();
	m_isoSpriteDefs = isoSpriteDefs;
}

//------------------------------------------------------------------------------------------------------------------------------
void IsoAnimDefenition::AddIsoSpriteDefenition(const IsoSpriteDefenition& isoSpriteDef)
{
	m_isoSpriteDefs.push_back(isoSpriteDef);
}

//------------------------------------------------------------------------------------------------------------------------------
const SpriteDefenition& IsoAnimDefenition::GetIsoSpriteAtTime(float seconds, const Vec3& direction) 
{
	switch (m_playbackType)
	{
	case SPRITE_ANIM_PLAYBACK_ONCE:
		return GetSpriteDefAtTime_Once(seconds, direction);
		break;
	case SPRITE_ANIM_PLAYBACK_LOOP:
		return GetSpriteDefAtTime_Loop(seconds, direction);
		break;
	case SPRITE_ANIM_PLAYBACK_PINGPONG:
		return GetSpriteDefAtTime_PingPong(seconds, direction);
		break;
	}

	return m_isoSpriteDefs[0].GetSpriteForLocalDirection(Vec3::BACK);
}

//------------------------------------------------------------------------------------------------------------------------------
const SpriteDefenition& IsoAnimDefenition::GetSpriteDefAtTime_Once(float seconds, const Vec3& direction)
{
	//Get total number of defs
	int numFrames = (m_endDefIndex - m_startDefIndex) + 1;
	float frameDuration = m_durationSeconds / numFrames;

	//Get the animation frame number
	int animFrameNum;
	if (seconds < m_durationSeconds)
	{
		animFrameNum = (int)floorf(seconds / frameDuration);
	}
	else
	{
		animFrameNum = m_endDefIndex;
	}

	//We now have the Def number (Which IsoSpriteDef corresponds to this frame)
	return m_isoSpriteDefs[animFrameNum].GetSpriteForLocalDirection(direction);
}

//------------------------------------------------------------------------------------------------------------------------------
const SpriteDefenition& IsoAnimDefenition::GetSpriteDefAtTime_Loop(float seconds, const Vec3& direction)
{
	//Get total number of frames
	int numFrames = (m_endDefIndex - m_startDefIndex) + 1;
	float frameDuration = m_durationSeconds / numFrames;

	//Get the animation frame number
	float timeInCycle = fmodf(seconds, m_durationSeconds);
	int animFrameNum = (int)floorf(timeInCycle / frameDuration);

	//We now have the Def number (Which IsoSpriteDef corresponds to this frame)
	return m_isoSpriteDefs[animFrameNum].GetSpriteForLocalDirection(direction);
}

//------------------------------------------------------------------------------------------------------------------------------
const SpriteDefenition& IsoAnimDefenition::GetSpriteDefAtTime_PingPong(float seconds, const Vec3& direction)
{
	//Get number of frames and each frame duration
	int numFrames = (m_endDefIndex - m_startDefIndex) * 2;
	int numBaseFrames = m_endDefIndex - m_startDefIndex + 1;
	float frameDuration = m_durationSeconds / numFrames;

	//Get time in Cycle and animation frame number at that time
	float timeInCycle = fmodf(seconds, m_durationSeconds);
	int animFrameNum = (int)floorf(timeInCycle / frameDuration);

	if (animFrameNum < numBaseFrames)
	{
		return m_isoSpriteDefs[animFrameNum + m_startDefIndex].GetSpriteForLocalDirection(direction);
	}
	else
	{
		int reverseIndexFromEnd = numBaseFrames - animFrameNum;		
		return m_isoSpriteDefs[reverseIndexFromEnd + m_endDefIndex].GetSpriteForLocalDirection(direction);
	}
}

