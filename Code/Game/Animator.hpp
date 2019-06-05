#pragma once
#include <map>
#include "Engine/Renderer/IsoSpriteDefenition.hpp"

enum eAnimationType
{
	ANIMATION_IDLE,
	ANIMATION_WALK,
	ANIMATION_ATTACK,
	ANIMATION_DIE,

	ANIMATION_COUNT,
};

class Animator
{
public:
	void Update(float deltaTime);

private:
	std::map<std::string, IsoSpriteDefenition*> m_animSet;
	IsoSpriteDefenition *m_currentAnimation;
	float m_totalElapsedTime;
};