//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Game/GameHandle.hpp"

//------------------------------------------------------------------------------------------------------------------------------
enum TaskTypeT
{
	FOLLOW,
	ATTACK,
	GATHER
};

//------------------------------------------------------------------------------------------------------------------------------
class RTSTask
{
public:
	RTSTask(TaskTypeT taskType);
	virtual ~RTSTask() {}
	virtual void Execute() = 0;

public:
	TaskTypeT m_taskType;
};

//------------------------------------------------------------------------------------------------------------------------------
class FollowTask : RTSTask
{
public:
	FollowTask(const GameHandle& thisUnit, const GameHandle& unitToFollow);
	~FollowTask();
	virtual void Execute();

public:
	GameHandle m_thisUnit;
	GameHandle m_unitToFollow;
};

//------------------------------------------------------------------------------------------------------------------------------
class AttackTask : RTSTask
{
public:
	AttackTask(const GameHandle& thisUnit, const GameHandle& unitToAttack);
	~AttackTask();
	virtual void Execute();

public:
	GameHandle m_thisUnit;
	GameHandle m_unitToAttack;
};