//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
//Game Systems
#include "Game/GameHandle.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class RTSCommand
{
public:
	RTSCommand();
	virtual ~RTSCommand() {}
	virtual void Execute() = 0;
};

//------------------------------------------------------------------------------------------------------------------------------
class CreateEntityCommand : RTSCommand
{
public:
	CreateEntityCommand(Vec2 createPosition);
	~CreateEntityCommand();

	virtual void Execute();

public:
	Vec2 m_createPosition;
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