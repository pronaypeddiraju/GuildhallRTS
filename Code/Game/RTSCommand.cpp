//------------------------------------------------------------------------------------------------------------------------------
#include "Game/RTSCommand.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"

//------------------------------------------------------------------------------------------------------------------------------
RTSCommand::RTSCommand()
{

}

//------------------------------------------------------------------------------------------------------------------------------
CreateEntityCommand::CreateEntityCommand(Vec2 createPosition)
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
	map->CreateEntity(m_createPosition);
}

//------------------------------------------------------------------------------------------------------------------------------
MoveCommand::MoveCommand(const GameHandle& unit, const Vec2& position)
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
