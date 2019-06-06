//------------------------------------------------------------------------------------------------------------------------------
#include "Game/RTSCommand.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"

//------------------------------------------------------------------------------------------------------------------------------
RTSCommand::RTSCommand(CommandTypeT commandType)
{
	m_commandType = commandType;
}

//------------------------------------------------------------------------------------------------------------------------------
CreateEntityCommand::CreateEntityCommand(Vec2 createPosition)
	:	RTSCommand(CREATE_ENTITY)
{
	m_createPosition = createPosition;
}

//------------------------------------------------------------------------------------------------------------------------------
CreateEntityCommand::~CreateEntityCommand()
{

}

VIRTUAL void CreateEntityCommand::Execute()
{
	Map* map = Game::s_gameReference->m_map;
	map->CreateEntity(m_createPosition, "peon");
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
