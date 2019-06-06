//------------------------------------------------------------------------------------------------------------------------------
#include "Game/GameInput.hpp"
//Engine Systems
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Math/Frustum.hpp"
#include "Engine/Math/Ray3D.hpp"
#include "Engine/Renderer/Camera.hpp"

//Game Systems
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/RTSCamera.hpp"
#include "Game/UIWidget.hpp"
#include "Game/GameHandle.hpp"
#include "Game/RTSCommand.hpp"
#include "Game/Entity.hpp"
#include "Game/RTSTask.hpp"

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

	//A11:
	UpdateGameControllerInput();
}

void GameInput::UpdateGameControllerInput()
{
	if (Game::s_gameReference->m_gameState != STATE_EDIT || Game::s_gameReference->m_gameState != STATE_PLAY)
	{
		return;
	}

	IntVec2 mousePosition = g_windowContext->GetClientMousePosition();
	IntVec2 clientBounds = g_windowContext->GetTrueClientBounds();
	Ray3D ray = m_game->m_RTSCam->ScreenPointToWorldRay(mousePosition, clientBounds);

	float entityTime;
	float mapTime;

	Map* map = m_game->m_map;

	Entity *entity = map->RaycastEntity(&entityTime, ray);
	if (map->RaycastTerrain(&mapTime, ray))
	{
		if (mapTime < entityTime)
		{
			entity = nullptr;
		}
	}

	for (int selectIndex = 0; selectIndex < (int)m_selectionHandles.size(); ++selectIndex)
	{
		Entity *selected = map->FindEntity(m_selectionHandles[selectIndex]);
		if (selected == nullptr)
		{
			m_selectionHandles[selectIndex] = GameHandle::INVALID;
		}
	}
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

	if (m_game->m_isPaused)
	{
		EventArgs args;

		std::vector<std::string> KeyValSplit = SplitStringOnDelimiter(eventString, '=');
		args.SetValue(KeyValSplit[0], KeyValSplit[1]);

		InputEvent event("MouseEvent", args);
		m_game->m_pauseParent->ProcessInput(event);
		return event.WasConsumed();
	}

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
	case STATE_PLAY:
	{
		m_mousePosLBDown = g_windowContext->GetClientMousePosition();
		m_selectionHandles.clear();
	}
	break;
	case STATE_EDIT:
	{

	}
	break;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------------------------
bool GameInput::HandleMouseLBUp()
{
	switch (m_game->m_gameState)
	{
	case STATE_PLAY:
	{
		m_mousePosLBUp = g_windowContext->GetClientMousePosition();

		if (GetDistanceSquared2D(Vec2(m_mousePosLBDown), Vec2(m_mousePosLBUp)) < 0.00001f)
		{
			SelectEntityAtClientPosition(m_mousePosLBDown);
		}
		else
		{
			SelectEntitiesInClientBox(m_mousePosLBDown, m_mousePosLBUp);
		}
	}
	break;
	default:
		break;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::SelectEntityAtClientPosition(const IntVec2& position)
{
	//Execute if the position was the same. This is a selection event
	IntVec2 clientBounds = g_windowContext->GetTrueClientBounds();
	Ray3D ray = m_game->m_RTSCam->ScreenPointToWorldRay(position, clientBounds);

	//Select the map if we hit that
	float terrainOut[2];
	m_game->m_map->RaycastTerrain(terrainOut, ray);

	float out[2];
	Entity* entity = m_game->m_map->RaycastEntity(out, ray);

	if (terrainOut[0] < out[0])
	{
		entity = nullptr;
		m_selectionHandles.clear();
		m_selectionHandles.push_back(GameHandle::INVALID);
	}

	if (entity)
	{
		entity->SetSelectable(true);
		m_selectionHandles.push_back(entity->GetHandle());
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::SelectEntitiesInClientBox(const IntVec2& boxStart, const IntVec2& boxEnd)
{
	//Create the box for the selection area in client space
	Frustum selectionFrustum = m_game->m_RTSCam->GetWorldFrustumFromClientRegion(AABB2(boxStart, boxEnd));
	//Frustum ndcFrustum = m_game->m_RTSCam->GetWorldFrustum();


	//Get all the selected Entities
	m_game->m_map->SelectEntitiesInFrustum(m_selectionHandles, selectionFrustum);
}

//------------------------------------------------------------------------------------------------------------------------------
bool GameInput::HandleMouseRBDown()
{
	for (int selectIndex = 0; selectIndex < (int)m_selectionHandles.size(); ++selectIndex)
	{
		if (m_selectionHandles[selectIndex] != GameHandle::INVALID)
		{
			IntVec2 mousePosition = g_windowContext->GetClientMousePosition();
			IntVec2 clientBounds = g_windowContext->GetTrueClientBounds();
			Ray3D ray = m_game->m_RTSCam->ScreenPointToWorldRay(mousePosition, clientBounds);

			//Select the map if we hit that
			float terrainOut[2];
			m_game->m_map->RaycastTerrain(terrainOut, ray);

			float out[2];
			Entity* entity = m_game->m_map->RaycastEntity(out, ray);

			if (!m_shiftPressed)
			{
				if (entity)
				{
					//Follow entity
					Vec3 dest = ray.GetPointAtTime(out[0]);
					FollowTask *followTask = new FollowTask(m_selectionHandles[selectIndex], entity->GetHandle());
					Entity* thisEntity = m_game->m_map->FindEntity(m_selectionHandles[selectIndex]);
					thisEntity->ClearTasks();
					thisEntity->EnqueueTask(reinterpret_cast<RTSTask*>(followTask));
				}
				else
				{
					Vec3 dest = ray.GetPointAtTime(terrainOut[0]);
					MoveCommand *cmd = new MoveCommand(m_selectionHandles[selectIndex], Vec2(dest.x, dest.y));
					m_game->m_map->FindEntity(m_selectionHandles[selectIndex])->StopFollow();
					m_game->EnqueueCommand(reinterpret_cast<RTSCommand*>(cmd));
				}
			}
			else
			{
				//Shift was pressed so queue a Task
				if (entity)
				{
					//Follow entity
					Vec3 dest = ray.GetPointAtTime(out[0]);
					FollowTask *followTask = new FollowTask(m_selectionHandles[selectIndex], entity->GetHandle());
					Entity* thisEntity = m_game->m_map->FindEntity(m_selectionHandles[selectIndex]);
					thisEntity->EnqueueTask(reinterpret_cast<RTSTask*>(followTask));
				}
			}
		}
	}

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
	case SPACE_KEY:
	{
		if (Game::s_gameReference->m_gameState != STATE_EDIT && Game::s_gameReference->m_gameState != STATE_PLAY)
		{
 			return;
		}

		//Create entity here using the command
		IntVec2 mousePos = g_windowContext->GetClientMousePosition();
		IntVec2 clientBounds = g_windowContext->GetTrueClientBounds();
		Ray3D ray = m_game->m_RTSCam->ScreenPointToWorldRay(mousePos, clientBounds);
		float out[2];
		uint count = m_game->m_map->RaycastTerrain(out, ray);
		if (count == 0)
		{
			return;
		}
		else
		{
			Vec3 camPosition = m_game->m_RTSCam->m_modelMatrix.GetTVector();
			Vec3 point = camPosition + ray.m_direction * out[0];

			//Use the command on game here
			CreateEntityCommand* command = new CreateEntityCommand(Vec2(point.x, point.y), PEON);
			m_game->EnqueueCommand(reinterpret_cast<RTSCommand*>(command));
		}
	}
	break;
	case M_KEY:
	{
		if (Game::s_gameReference->m_gameState != STATE_EDIT && Game::s_gameReference->m_gameState != STATE_PLAY)
		{
			return;
		}

		//Create entity here using the command
		IntVec2 mousePos = g_windowContext->GetClientMousePosition();
		IntVec2 clientBounds = g_windowContext->GetTrueClientBounds();
		Ray3D ray = m_game->m_RTSCam->ScreenPointToWorldRay(mousePos, clientBounds);
		float out[2];
		uint count = m_game->m_map->RaycastTerrain(out, ray);
		if (count == 0)
		{
			return;
		}
		else
		{
			Vec3 camPosition = m_game->m_RTSCam->m_modelMatrix.GetTVector();
			Vec3 point = camPosition + ray.m_direction * out[0];

			//Use the command on game here
			CreateEntityCommand* command = new CreateEntityCommand(Vec2(point.x, point.y), WARRIOR);
			m_game->EnqueueCommand(reinterpret_cast<RTSCommand*>(command));
		}
	}
	break;
	case LSHIFT_KEY:
	{
		m_shiftPressed = true;
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
		case LSHIFT_KEY:
		{
			m_shiftPressed = false;
		}
		break;
		default:
		break;
	}
}

