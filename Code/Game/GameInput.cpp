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

	UpdateGameControllerInput();

	if (m_towncenterSpawnSelect || m_hutSpawnSelect)
	{
		SetupMapCastPosition();
	}
}

void GameInput::SetupMapCastPosition()
{
	IntVec2 mousePosition = g_windowContext->GetClientMousePosition();
	IntVec2 clientBounds = g_windowContext->GetTrueClientBounds();
	Ray3D ray = m_game->m_RTSCam->ScreenPointToWorldRay(mousePosition, clientBounds);

	//Select the map if we hit that
	float terrainOut[2];
	m_game->m_map->RaycastTerrain(terrainOut, ray);

	Vec3 dest = ray.GetPointAtTime(terrainOut[0]);
	m_terrainCastLocation = Vec2(dest.x, dest.y);
}

void GameInput::UpdateGameControllerInput()
{
	if (Game::s_gameReference->m_gameState != STATE_EDIT && Game::s_gameReference->m_gameState != STATE_PLAY)
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
	if (entity != nullptr && entity->IsAlive())
	{
		m_hoverHandle = entity->GetHandle();
	}
	else
	{
		m_hoverHandle = GameHandle::INVALID;
	}

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
		m_LMousePressed = true;
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
		m_LMousePressed = false;
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

	if (entity != nullptr && entity->IsAlive())
	{
		if (entity->GetTeam() == m_game->GetCurrentTeam())
		{
			entity->SetSelectable(true);
			m_selectionHandles.push_back(entity->GetHandle());
		}
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
	if (Game::s_gameReference->m_gameState != STATE_PLAY && Game::s_gameReference->m_gameState != STATE_EDIT)
		return false;

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

			Entity* thisEntity = m_game->m_map->FindEntity(m_selectionHandles[selectIndex]);

			if (m_shiftPressed)
			{
				thisEntity->ClearTasks();
			}

			if (entity)
			{

				thisEntity->ResetTaskData();

				if (entity->GetTeam() == thisEntity->GetTeam() && !entity->IsBuildingType())
				{
					//Follow entity
					FollowTask *followTask = new FollowTask(m_selectionHandles[selectIndex], entity->GetHandle());
					thisEntity->EnqueueTask(reinterpret_cast<RTSTask*>(followTask));
				}
				else if (entity->GetTeam() != thisEntity->GetTeam() && !entity->IsResource())
				{
					//Fuck up your enemies						
					AttackTask *attackTask = new AttackTask(m_selectionHandles[selectIndex], entity->GetHandle());
					thisEntity->EnqueueTask(reinterpret_cast<RTSTask*>(attackTask));
				}
				else if (entity->IsResource())
				{
					if (thisEntity->GetType() == PEON || thisEntity->GetType() == GOBLIN)
					{
						//Gather some shit
						GatherTask *gatherTask = new GatherTask(m_selectionHandles[selectIndex], entity->GetHandle());
						thisEntity->EnqueueTask(reinterpret_cast<RTSTask*>(gatherTask));
					}
					else
					{
						MoveCommand *cmd = new MoveCommand(m_selectionHandles[selectIndex], entity->GetPosition());
						m_game->m_map->FindEntity(m_selectionHandles[selectIndex])->StopFollow();
						thisEntity->ResetTaskData();
						m_game->EnqueueCommand(reinterpret_cast<RTSCommand*>(cmd));
					}
				}
				else if (entity->IsBuildingType())
				{
					if (thisEntity->GetType() == PEON || thisEntity->GetType() == GOBLIN)
					{
						if (entity->GetHealth() < entity->GetMaxHealth())
						{
							thisEntity->SetUnitToBuild(entity);
						}
						else
						{
							thisEntity->MoveTo(entity->GetPosition());
						}
					}
					else
					{
						thisEntity->MoveTo(entity->GetPosition());
					}
				}
			}
			else
			{
				Vec3 dest = ray.GetPointAtTime(terrainOut[0]);
				m_terrainCastLocation = Vec2(dest.x, dest.y);

				if (IntVec2(m_terrainCastLocation).IsInBounds(Game::s_gameReference->m_map->m_tileDimensions))
				{
					MoveCommand *cmd = new MoveCommand(m_selectionHandles[selectIndex], m_terrainCastLocation);
					m_game->m_map->FindEntity(m_selectionHandles[selectIndex])->StopFollow();
					thisEntity->ResetTaskData();
					m_game->EnqueueCommand(reinterpret_cast<RTSCommand*>(cmd));
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
	IntVec2 mousePosition = g_windowContext->GetClientMousePosition();
	IntVec2 clientBounds = g_windowContext->GetTrueClientBounds();
	Ray3D ray = m_game->m_RTSCam->ScreenPointToWorldRay(mousePosition, clientBounds);

	float terrainOut[2];
	float out[2];
	Entity* entity = nullptr;

	if (Game::s_gameReference->m_gameState == STATE_EDIT && Game::s_gameReference->m_gameState == STATE_PLAY)
	{
		//Select the map if we hit that
		m_game->m_map->RaycastTerrain(terrainOut, ray);
		entity = m_game->m_map->RaycastEntity(out, ray);
	}

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

		int team = Game::s_gameReference->GetCurrentTeam();
		if (team == 1)
		{
			SpawnUnit(PEON);
		}
		else
		{
			SpawnUnit(GOBLIN);
		}

	}
	break;
	case M_KEY:
	{
		if (Game::s_gameReference->m_gameState != STATE_EDIT && Game::s_gameReference->m_gameState != STATE_PLAY)
		{
			return;
		}

		SpawnUnit(WARRIOR);
	}
	break;
	case N_KEY:
	{
		if (Game::s_gameReference->m_gameState != STATE_EDIT && Game::s_gameReference->m_gameState != STATE_PLAY)
		{
			return;
		}

		SpawnUnit(TREE);
	}
	break;
	case B_KEY:
	{
		if (Game::s_gameReference->m_gameState != STATE_EDIT && Game::s_gameReference->m_gameState != STATE_PLAY)
		{
			return;
		}

		MakeBuilding();
	}
	break;
	case V_KEY:
	{
		if (Game::s_gameReference->m_gameState != STATE_EDIT && Game::s_gameReference->m_gameState != STATE_PLAY)
		{
			return;
		}

		MakeHut();
	}
	break;
	case LSHIFT_KEY:
	{
		m_shiftPressed = true;
	}
	break;
	case T_KEY:
	{
		int currentTeam = m_game->GetCurrentTeam();

		switch (currentTeam)
		{
		case 1:
		{
			m_game->SetCurrentTeam(2);
			SetTeamForSelectedEntities(2);
		}
		break;
		case 2:
		{
			m_game->SetCurrentTeam(1);
			SetTeamForSelectedEntities(1);
		}
		break;
		default:
			break;
		}
		
	}
	break;
	case NUM_2:
	{
		for (int selectIndex = 0; selectIndex < (int)m_selectionHandles.size(); ++selectIndex)
		{
			if (m_selectionHandles[selectIndex] != GameHandle::INVALID)
			{
				Entity* thisEntity = m_game->m_map->FindEntity(m_selectionHandles[selectIndex]);

				Vec3 dest = ray.GetPointAtTime(terrainOut[0]);
				m_terrainCastLocation = Vec2(dest.x, dest.y);

				MoveCommand *cmd = new MoveCommand(m_selectionHandles[selectIndex], m_terrainCastLocation);
				m_game->m_map->FindEntity(m_selectionHandles[selectIndex])->StopFollow();
				thisEntity->ResetTaskData();
				m_game->EnqueueCommand(reinterpret_cast<RTSCommand*>(cmd));
			}
		}
	}
	break;
	case NUM_3:
	{
		for (int selectIndex = 0; selectIndex < (int)m_selectionHandles.size(); ++selectIndex)
		{
			if (m_selectionHandles[selectIndex] != GameHandle::INVALID)
			{
				Entity* thisEntity = m_game->m_map->FindEntity(m_selectionHandles[selectIndex]);

				if (m_shiftPressed)
				{
					thisEntity->ClearTasks();
				}

				if (entity)
				{
					thisEntity->ResetTaskData();
					if (entity->GetTeam() != thisEntity->GetTeam() && !entity->IsResource())
					{
						//Fuck up your enemies						
						AttackTask *attackTask = new AttackTask(m_selectionHandles[selectIndex], entity->GetHandle());
						thisEntity->EnqueueTask(reinterpret_cast<RTSTask*>(attackTask));
					}
				}
				else
				{
					Vec3 dest = ray.GetPointAtTime(terrainOut[0]);
					m_terrainCastLocation = Vec2(dest.x, dest.y);

					MoveCommand *cmd = new MoveCommand(m_selectionHandles[selectIndex], m_terrainCastLocation);
					m_game->m_map->FindEntity(m_selectionHandles[selectIndex])->StopFollow();
					thisEntity->ResetTaskData();
					m_game->EnqueueCommand(reinterpret_cast<RTSCommand*>(cmd));
				}
			}
		}
	}
	break;
	case NUM_4:
	{
		for (int selectIndex = 0; selectIndex < (int)m_selectionHandles.size(); ++selectIndex)
		{
			if (m_selectionHandles[selectIndex] != GameHandle::INVALID)
			{
				Entity* thisEntity = m_game->m_map->FindEntity(m_selectionHandles[selectIndex]);

				if (m_shiftPressed)
				{
					thisEntity->ClearTasks();
				}

				if (entity)
				{
					thisEntity->ResetTaskData();
					
					if (entity->IsResource())
					{
						if (thisEntity->GetType() == PEON)
						{
							//Gather some shit
							GatherTask *gatherTask = new GatherTask(m_selectionHandles[selectIndex], entity->GetHandle());
							thisEntity->EnqueueTask(reinterpret_cast<RTSTask*>(gatherTask));
						}
						else
						{
							MoveCommand *cmd = new MoveCommand(m_selectionHandles[selectIndex], entity->GetPosition());
							m_game->m_map->FindEntity(m_selectionHandles[selectIndex])->StopFollow();
							thisEntity->ResetTaskData();
							m_game->EnqueueCommand(reinterpret_cast<RTSCommand*>(cmd));
						}
					}
				}
				else
				{
					Vec3 dest = ray.GetPointAtTime(terrainOut[0]);
					m_terrainCastLocation = Vec2(dest.x, dest.y);

					MoveCommand *cmd = new MoveCommand(m_selectionHandles[selectIndex], m_terrainCastLocation);
					m_game->m_map->FindEntity(m_selectionHandles[selectIndex])->StopFollow();
					thisEntity->ResetTaskData();
					m_game->EnqueueCommand(reinterpret_cast<RTSCommand*>(cmd));
				}
			}
		}
	}
	break;
	case NUM_5:
	{
		MakeBuilding();
	}
	break;
	case NUM_6:
	{
		TrainUnit();
	}
	break;
	case K_KEY:
	{
		if (m_selectionHandles.size() > 0)
		{
			for (int entityIndex = 0; entityIndex < m_selectionHandles.size(); entityIndex++)
			{
				entity = m_game->m_map->FindEntity(m_selectionHandles[entityIndex]);
				entity->SetHealth(0.f);
			}
		}
	}
	case L_KEY:
	{
		m_game->m_disableAI = !m_game->m_disableAI;
	}
	break;
	}
}

void GameInput::MakeBuilding()
{
	int team = m_game->GetCurrentTeam() - 1;

	if (m_towncenterSpawnSelect)
	{
		for (int i = 0; i < m_selectionHandles.size(); i++)
		{
			if (m_selectionHandles[i] != GameHandle::INVALID)
			{
				Entity* thisEntity = m_game->m_map->FindEntity(m_selectionHandles[i]);
				if (thisEntity->GetType() == PEON || thisEntity->GetType() == GOBLIN)
				{
					Vec2 buildPos = GetCorrectedMapPosition(m_terrainCastLocation, m_game->m_map->m_tileDimensions, m_game->m_map->m_townCenterOcc);

					if (m_game->m_map->IsRegionOccupied(buildPos, m_game->m_map->m_townCenterOcc))
						return;

					if (m_game->m_teamResource[team] < m_game->m_map->GetTownCenterCost())
					{
						return;
					}
					else
					{
						m_game->m_teamResource[team] -= m_game->m_map->GetTownCenterCost();
					}

					//build some shit
					BuildTask *buildTask = new BuildTask(m_selectionHandles[i], buildPos, TOWNCENTER);
					thisEntity->EnqueueTask(reinterpret_cast<RTSTask*>(buildTask));

					m_game->m_map->SetOccupancyForUnit(buildPos, m_game->m_map->m_townCenterOcc, true);

					m_towncenterSpawnSelect = false;
					break;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < m_selectionHandles.size(); i++)
		{
			if (m_selectionHandles[i] != GameHandle::INVALID)
			{
				Entity* thisEntity = m_game->m_map->FindEntity(m_selectionHandles[i]);
				if (thisEntity->GetType() == PEON || thisEntity->GetType() == GOBLIN)
				{
					if (m_game->m_teamResource[team] < m_game->m_map->GetTownCenterCost())
					{
						return;
					}

					if (!m_towncenterSpawnSelect)
					{
						m_towncenterSpawnSelect = true;
					}
					break;
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::MakeHut()
{
	int team = m_game->GetCurrentTeam() - 1;

	if (m_hutSpawnSelect)
	{
		for (int i = 0; i < m_selectionHandles.size(); i++)
		{
			if (m_selectionHandles[i] != GameHandle::INVALID)
			{
				Entity* thisEntity = m_game->m_map->FindEntity(m_selectionHandles[i]);
				if (thisEntity->GetType() == PEON || thisEntity->GetType() == GOBLIN)
				{
					Vec2 buildPos = GetCorrectedMapPosition(m_terrainCastLocation, m_game->m_map->m_tileDimensions, m_game->m_map->m_hutOcc);

					if (m_game->m_map->IsRegionOccupied(buildPos, m_game->m_map->m_hutOcc))
						return;

					if (m_game->m_teamResource[team] < m_game->m_map->GetHutCost())
					{
						return;
					}
					else
					{
						m_game->m_teamResource[team] -= m_game->m_map->GetHutCost();
					}

					//build some shit
					BuildTask *buildTask = new BuildTask(m_selectionHandles[i], buildPos, HUT);
					thisEntity->EnqueueTask(reinterpret_cast<RTSTask*>(buildTask));

					m_game->m_map->SetOccupancyForUnit(buildPos, m_game->m_map->m_hutOcc, true);

					m_hutSpawnSelect = false;
					break;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < m_selectionHandles.size(); i++)
		{
			if (m_selectionHandles[i] != GameHandle::INVALID)
			{
				Entity* thisEntity = m_game->m_map->FindEntity(m_selectionHandles[i]);
				if (thisEntity->GetType() == PEON || thisEntity->GetType() == GOBLIN)
				{
					if (m_game->m_teamResource[team] < m_game->m_map->GetHutCost())
					{
						return;
					}

					if (!m_hutSpawnSelect)
					{
						m_hutSpawnSelect = true;
					}
					break;
				}
			}
		}
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

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::SetTeamForSelectedEntities(int teamNum)
{
	for (int i = 0; i < (int)m_selectionHandles.size(); i++)
	{
		Entity* entity = Game::s_gameReference->m_map->FindEntity(m_selectionHandles[i]);
		if(entity != nullptr)
		{
			entity->SetTeam(teamNum);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void GameInput::SpawnUnit(EntityTypeT type, const Vec2& buildPos)
{
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

		IntVec2 mapBounds = m_game->m_map->m_tileDimensions;

		Vec2 pointOnMap = GetCorrectedMapPosition(Vec2(point.x, point.y), mapBounds, IntVec2(0, 0));

		CreateEntityCommand* command;

		if (type == TOWNCENTER)
		{
			pointOnMap = GetCorrectedMapPosition(buildPos, mapBounds, m_game->m_map->m_townCenterOcc);
		}

		bool result = m_game->m_map->IsRegionOccupied(pointOnMap, IntVec2(1, 1));
		if (result)
		{
			return;
		}

		//Set occupancy for tree
		if (type == TREE)
		{
			m_game->m_map->SetOccupancyForUnit(pointOnMap, IntVec2(1, 1), true);
		}

		command = new CreateEntityCommand(pointOnMap, type);
		m_game->EnqueueCommand(reinterpret_cast<RTSCommand*>(command));
	}
}

Vec2 GameInput::GetCorrectedMapPosition(Vec2 position, IntVec2 limits, IntVec2 occupancy)
{
	IntVec2 clampPosition;

	clampPosition.x = (int)Clamp(position.x, 0.f, (float)limits.x);
	clampPosition.y = (int)Clamp(position.y, 0.f, (float)limits.y);

	if (clampPosition.x <= (occupancy.x /2))
	{
		clampPosition.x = (int)((float)occupancy.x / 2.f);
	}
	else if (clampPosition.x >= (limits.x - occupancy.x))
	{
		clampPosition.x -= occupancy.x;
	}

	if (clampPosition.y <= (occupancy.y / 2))
	{
		clampPosition.y = (int)((float)occupancy.y / 2.f);
	}
	else if (clampPosition.y >= (limits.y - occupancy.y))
	{
		clampPosition.y -= occupancy.y;
	}

	Vec2 correctedPos = Vec2(clampPosition.x + 0.5f, clampPosition.y + 0.5f);
	return correctedPos;
}

void GameInput::TrainUnit()
{
	for (int selectIndex = 0; selectIndex < (int)m_selectionHandles.size(); ++selectIndex)
	{
		if (m_selectionHandles[selectIndex] != GameHandle::INVALID)
		{
			Entity* thisEntity = m_game->m_map->FindEntity(m_selectionHandles[selectIndex]);

			if (thisEntity->GetType() == TOWNCENTER || thisEntity->GetType() == HUT)
			{
				int team = m_game->GetCurrentTeam() - 1;

				if (m_game->m_teamCurrentSupply[team] >= m_game->m_teamMaxSupply[team])
					return;

				if (team == 0)
				{
					if (m_game->m_teamResource[team] >= m_game->m_map->GetPeonCost())
					{
						thisEntity->SetIsTrainingUnit(true);

						m_game->m_teamResource[team] -= m_game->m_map->GetPeonCost();
						m_game->m_teamCurrentSupply[team] += 1;
					}
				}
				else
				{
					if (m_game->m_teamResource[team] >= m_game->m_map->GetGoblinCost())
					{
						thisEntity->SetIsTrainingUnit(true);

						m_game->m_teamResource[team] -= m_game->m_map->GetGoblinCost();
						m_game->m_teamCurrentSupply[team] += 1;
					}
				}
			}
		}
	}
}
