//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Game/GameHandle.hpp"
#include "Engine/Math/Vec2.hpp"

//------------------------------------------------------------------------------------------------------------------------------
enum TaskTypeT
{
	FOLLOW,
	ATTACK,
	GATHER,
	BUILD
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

//------------------------------------------------------------------------------------------------------------------------------
class GatherTask : RTSTask
{
public:
	GatherTask(const GameHandle& thisUnit, const GameHandle& unitToGather);
	~GatherTask();
	virtual void Execute();

public:
	GameHandle m_thisUnit;
	GameHandle m_unitToGather;
};

//------------------------------------------------------------------------------------------------------------------------------
class BuildTask : RTSTask
{
public:
	BuildTask(const GameHandle& thisUnit, const Vec2& buildLocation);
	~BuildTask();
	virtual void Execute();

public:
	GameHandle m_thisUnit;
	Vec2 m_buildLocation;
};