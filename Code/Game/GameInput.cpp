//------------------------------------------------------------------------------------------------------------------------------
#include "Game/GameInput.hpp"
//Engine Systems
#include "Engine/Renderer/Camera.hpp"
//Game Systems
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/RTSCamera.hpp"

//------------------------------------------------------------------------------------------------------------------------------
GameInput::GameInput(Game* game)
{
	m_game = game;
	m_framePan = Vec2::ZERO;
	m_frameZoomDelta = 0.f;
}

//------------------------------------------------------------------------------------------------------------------------------
GameInput::~GameInput()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::BeginFrame()
{
	
}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::Update( float deltaTime )
{
	UNUSED(deltaTime);
	//Update the inputs and also check if your mouse is in a place where you should be doing something
}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::EndFrame()
{
	m_framePan = Vec2::ZERO;
	m_frameZoomDelta = 0.f;
	
	if(!m_isRotating)
	{
		m_frameRotation = 0.f;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 GameInput::GetFramePan() const
{
	return m_framePan;
}

//------------------------------------------------------------------------------------------------------------------------------
float GameInput::GetFrameRotation() const
{
	return m_frameRotation;
}

//------------------------------------------------------------------------------------------------------------------------------
bool GameInput::IsRotating() const
{
	return m_isRotating;
}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::SetFramePan( Vec2 panAmount )
{
	m_framePan = panAmount;
}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::HandleKeyPressed( unsigned char keyCode )
{
	switch( keyCode )
	{
	case A_KEY:
	{
		/*
		Vec2 camForward = Vec2(0.f, 1.f);
		camForward.RotateDegrees(m_game->m_RTSCam->m_angle);

		m_game->m_RTSCam->m_focalPoint -= camForward * m_cameraSpeed;

		m_game->m_RTSCam->m_focalPoint.ClampVector(m_game->m_RTSCam->m_focalPoint, m_game->m_map->GetXYBounds().m_minBounds, m_game->m_map->GetXYBounds().m_maxBounds);
		*/

		Vec2 camForward = Vec2(0.f, 1.f);
		camForward.RotateDegrees(m_game->m_RTSCam->m_angle);
		m_framePan -= camForward * m_keyboardPanSpeed;
	}
	break;
	case W_KEY:
	{
		/*
		Vec2 camRight = Vec2(1.f, 0.f);
		camRight.RotateDegrees(m_game->m_RTSCam->m_angle);

		m_game->m_RTSCam->m_focalPoint -= camRight * m_cameraSpeed;

		m_game->m_RTSCam->m_focalPoint.ClampVector(m_game->m_RTSCam->m_focalPoint, m_game->m_map->GetXYBounds().m_minBounds, m_game->m_map->GetXYBounds().m_maxBounds);
		*/

		Vec2 camRight = Vec2(1.f, 0.f);
		camRight.RotateDegrees(m_game->m_RTSCam->m_angle);
		m_framePan -= camRight * m_keyboardPanSpeed;
	}
	break;
	case S_KEY:
	{
		/*
		Vec2 camRight = Vec2(1.f, 0.f);
		camRight.RotateDegrees(m_game->m_RTSCam->m_angle);

		m_game->m_RTSCam->m_focalPoint += camRight * m_cameraSpeed;

		m_game->m_RTSCam->m_focalPoint.ClampVector(m_game->m_RTSCam->m_focalPoint, m_game->m_map->GetXYBounds().m_minBounds, m_game->m_map->GetXYBounds().m_maxBounds);
		*/

		Vec2 camRight = Vec2(1.f, 0.f);
		camRight.RotateDegrees(m_game->m_RTSCam->m_angle);
		m_framePan += camRight * m_keyboardPanSpeed;
	}
	break;
	case D_KEY:
	{
		/*
		Vec2 camForward = Vec2(0.f, 1.f);
		camForward.RotateDegrees(m_game->m_RTSCam->m_angle);

		m_game->m_RTSCam->m_focalPoint += camForward * m_cameraSpeed;

		m_game->m_RTSCam->m_focalPoint.ClampVector(m_game->m_RTSCam->m_focalPoint, m_game->m_map->GetXYBounds().m_minBounds, m_game->m_map->GetXYBounds().m_maxBounds);
		*/

		Vec2 camForward = Vec2(0.f, 1.f);
		camForward.RotateDegrees(m_game->m_RTSCam->m_angle);
		m_framePan += camForward * m_keyboardPanSpeed;
	}
	break;
	case LCTRL_KEY:
	{
		if(!m_isRotating)
		{
			m_isRotating = true;
		}

		m_frameRotation += m_rotationSpeed;
	}
	break;
	default:
	break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::HandleKeyReleased( unsigned char keyCode )
{
	switch (keyCode)
	{
		case LCTRL_KEY:
		{
			m_isRotating = false;
		}
		break;
		default:
		break;
	}
}

