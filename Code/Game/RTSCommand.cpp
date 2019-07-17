//------------------------------------------------------------------------------------------------------------------------------
#include "Game/RTSCommand.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/AIController.hpp"

//------------------------------------------------------------------------------------------------------------------------------
RTSCommand::RTSCommand(CommandTypeT commandType)
{
	m_commandType = commandType;
}

//------------------------------------------------------------------------------------------------------------------------------
CreateEntityCommand::CreateEntityCommand(Vec2 createPosition, EntityTypeT entityType)
	:	RTSCommand(CREATE_ENTITY)
{
	m_createPosition = createPosition;
	m_entityType = entityType;

	switch (entityType)
	{
	case PEON:
		m_team = 1;
		break;
	case WARRIOR:
		m_team = 1;
		break;
	case TREE:
		m_team = 0;
		break;
	case TOWNCENTER:
		break;
	case HUT:
		break;
	case GOBLIN:
		m_team = 2;
		break;
	default:
		break;
	}

}

//------------------------------------------------------------------------------------------------------------------------------
CreateEntityCommand::~CreateEntityCommand()
{

}

VIRTUAL void CreateEntityCommand::Execute()
{
	Map* map = Game::s_gameReference->m_map;
	int team;
	if (m_team == 2)
	{
		team = Game::s_gameReference->m_map->m_AIController->m_AITeam;
	}
	else
	{
		team = Game::s_gameReference->GetCurrentTeam();
	}

	map->CreateEntity(m_createPosition, m_entityType, team);
}

//------------------------------------------------------------------------------------------------------------------------------
MoveCommand::MoveCommand(const GameHandle& unit, const Vec2& position)
	: RTSCommand(MOVE_ENTITY)
{
	m_unit = unit;
	m_position = position;
}

//------------------------------------------------------------------------------------------------------------------------------
MoveCommand::~MoveCommand()
{

}

//------------------------------------------------------------------------
VIRTUAL void MoveCommand::Execute()
{
	Map* map = Game::s_gameReference->m_map;
	Entity *entity = map->FindEntity(m_unit);	
	if (entity != nullptr) 
	{
		entity->PathTo(m_position);
	}
}
