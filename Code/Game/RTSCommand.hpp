//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
//Game Systems
#include "Game/GameHandle.hpp"
#include "Game/Map.hpp"

//------------------------------------------------------------------------------------------------------------------------------
enum CommandTypeT
{
	CREATE_ENTITY,
	MOVE_ENTITY,
};

//------------------------------------------------------------------------------------------------------------------------------
class RTSCommand
{
public:
	RTSCommand(CommandTypeT commandType);
	virtual ~RTSCommand() {}
	virtual void Execute() = 0;

public:
	CommandTypeT m_commandType;
};

//------------------------------------------------------------------------------------------------------------------------------
class CreateEntityCommand : RTSCommand
{
public:
	CreateEntityCommand(Vec2 createPosition, EntityTypeT entityType);
	~CreateEntityCommand();

	virtual void Execute();

public:
	Vec2 m_createPosition;
	EntityTypeT m_entityType;
	int m_team = 0;
};

//------------------------------------------------------------------------------------------------------------------------------
class MoveCommand : RTSCommand
{
public:
	MoveCommand(const GameHandle& unit, const Vec2& position);
	~MoveCommand();
	virtual void Execute();

public:
	GameHandle m_unit;
	Vec2 m_position;
};