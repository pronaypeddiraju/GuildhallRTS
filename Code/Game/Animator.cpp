#include "Game/Animator.hpp"
#include "Game/IsoAnimDefenition.hpp"
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
void Animator::Update(float deltaTime)
{
	m_totalElapsedTime += deltaTime;
}
