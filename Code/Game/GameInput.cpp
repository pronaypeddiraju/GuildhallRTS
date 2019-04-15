//------------------------------------------------------------------------------------------------------------------------------
#include "Game/GameInput.hpp"
//Engine Systems
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Renderer/Camera.hpp"

//Game Systems
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/RTSCamera.hpp"
#include "Game/UIWidget.hpp"

//------------------------------------------------------------------------------------------------------------------------------
GameInput::GameInput(Game* game)
{
	m_game = game;
	m_framePan = Vec2::ZERO;

	m_screenBounds = AABB2(Vec2(0.f, 0.f), Vec2(1280.f, 720.f));

	m_mouseButtonLeft.ResetButton();
	m_mouseButtonRight.ResetButton();
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
	//Update the inputs for keyboard input
	UpdateKeyBoardPan();

	//Update the mouse position and check if you need to pan
	UpdateMousePan();

	m_framePan *= deltaTime;

	m_game->m_RTSCam->SetZoomDelta(m_frameZoomDelta);

	if(!m_isRotating && (m_game->m_RTSCam->m_angle > m_game->m_RTSCam->m_defaultAngle))
	{
		m_frameRotation -= m_rotationSpeed;
	}

	m_frameRotation *= deltaTime;

}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::UpdateKeyBoardPan()
{
	Vec2 camForward = Vec2(0.f, 1.f);
	Vec2 camRight = Vec2(1.f, 0.f);

	camForward.RotateDegrees(m_game->m_RTSCam->m_angle);
	camRight.RotateDegrees(m_game->m_RTSCam->m_angle);

	//Check right, left
	if(m_APressed)
	{
		m_framePan -= camForward * m_keyboardPanSpeed;
	}

	if(m_DPressed)
	{
		m_framePan += camForward * m_keyboardPanSpeed;
	}

	if(m_WPressed)
	{
		m_framePan -= camRight * m_keyboardPanSpeed;
	}

	if(m_SPressed)
	{
		m_framePan += camRight * m_keyboardPanSpeed;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::UpdateMousePan()
{
	Vec2 camForward = Vec2(0.f, 1.f);
	Vec2 camRight = Vec2(1.f, 0.f);

	camForward.RotateDegrees(m_game->m_RTSCam->m_angle);
	camRight.RotateDegrees(m_game->m_RTSCam->m_angle);

	IntVec2 mousePos = g_windowContext->GetClientMousePosition();

	Vec2 screenPos = m_game->GetClientToUIScreenPosition2D(mousePos, g_windowContext->GetClientBounds());

	if(screenPos.x < (m_screenBounds.m_minBounds.x + m_edgePanDistance))
	{
		m_framePan -= camForward * m_edgePanSpeed;
	}
	else if(screenPos.x > (m_screenBounds.m_maxBounds.x- m_edgePanDistance))
	{
		m_framePan += camForward * m_edgePanSpeed;
	}

	if(screenPos.y > (m_screenBounds.m_maxBounds.y- m_edgePanDistance))
	{
		m_framePan -= camRight * m_edgePanSpeed;
	}
	else if(screenPos.y < (m_screenBounds.m_minBounds.y + m_edgePanDistance))
	{
		m_framePan += camRight * m_edgePanSpeed;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::EndFrame()
{
	m_framePan = Vec2::ZERO;
	m_frameZoomDelta = 0.f;
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
void GameInput::SetScreenBounds( const AABB2& screenBounds )
{
	m_screenBounds = screenBounds;
}

//------------------------------------------------------------------------------------------------------------------------------
float GameInput::GetScreenWidth()
{
	float width = m_screenBounds.m_maxBounds.x - m_screenBounds.m_minBounds.x;
	return width;
}

//------------------------------------------------------------------------------------------------------------------------------
float GameInput::GetScreenHeight()
{
	float height = m_screenBounds.m_maxBounds.y - m_screenBounds.m_minBounds.y;
	return height;
}

//------------------------------------------------------------------------------------------------------------------------------
bool GameInput::HandleMouseLBDown()
{
	std::string eventString = "clickType=LBDown";

	switch (m_game->m_gameState)
	{
	case STATE_MENU:
	{
		EventArgs args;

		std::vector<std::string> KeyValSplit = SplitStringOnDelimiter(eventString, '=');
		args.SetValue(KeyValSplit[0], KeyValSplit[1]);

		InputEvent event("MouseEvent", args);
		m_game->m_menuParent->ProcessInput(event);
		return event.WasConsumed();
	}
	break;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------------------------
bool GameInput::HandleMouseLBUp()
{
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------
bool GameInput::HandleMouseRBDown()
{
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------
bool GameInput::HandleMouseRBUp()
{
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------
bool GameInput::HandleMouseScroll( float wheelDelta )
{
	m_frameZoomDelta -= wheelDelta;

	m_frameZoomDelta = Clamp(m_frameZoomDelta, MIN_ZOOM_STEPS, MAX_ZOOM_STEPS);

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::HandleKeyPressed( unsigned char keyCode )
{
	switch( keyCode )
	{
	case A_KEY:
	{
		if(!m_APressed)
		{
			m_APressed = true;
		}
	}
	break;
	case W_KEY:
	{
		if(!m_WPressed)
		{
			m_WPressed = true;
		}
	}
	break;
	case S_KEY:
	{
		if(!m_SPressed)
		{
			m_SPressed = true;
		}
	}
	break;
	case D_KEY:
	{
		if(!m_DPressed)
		{
			m_DPressed = true;
		}
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
		case A_KEY:
		{
			m_APressed = false;
		}
		break;
		case W_KEY:
		{
			m_WPressed = false;
		}
		break;
		case S_KEY:
		{
			m_SPressed = false;
		}
		break;
		case D_KEY:
		{
			m_DPressed = false;
		}
		break;
		default:
		break;
	}
}

