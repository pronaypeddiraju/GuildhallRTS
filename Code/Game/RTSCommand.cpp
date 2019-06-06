//------------------------------------------------------------------------------------------------------------------------------
#include "Game/RTSCommand.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"

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
}

//------------------------------------------------------------------------------------------------------------------------------
CreateEntityCommand::~CreateEntityCommand()
{

}

VIRTUAL void CreateEntityCommand::Execute()
{
	switch (m_entityType)
	{
	case PEON:
	{
		Map* map = Game::s_gameReference->m_map;
		map->CreateEntity(m_createPosition, "peon", *Game::s_gameReference->m_peonSheet, m_entityType);
	}
	break;
	case WARRIOR:
	{
		Map* map = Game::s_gameReference->m_map;
		map->CreateEntity(m_createPosition, "warrior", *Game::s_gameReference->m_warriorSheet, m_entityType);
	}
	break;
	default:
		break;
	}
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
		entity->MoveTo(m_position);
	}
}
