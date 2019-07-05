#include "Game/RTSTask.hpp"
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
//Game Systems
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"

//------------------------------------------------------------------------------------------------------------------------------
RTSTask::RTSTask(TaskTypeT taskType)
{
	m_taskType = taskType;
}

//------------------------------------------------------------------------------------------------------------------------------
FollowTask::FollowTask(const GameHandle& thisUnit, const GameHandle& unitToFollow)
	: RTSTask(FOLLOW)
{
	m_thisUnit = thisUnit;
	m_unitToFollow = unitToFollow;
}

//------------------------------------------------------------------------------------------------------------------------------
FollowTask::~FollowTask()
{

}

//------------------------------------------------------------------------------------------------------------------------------
VIRTUAL void FollowTask::Execute()
{
	Map* map = Game::s_gameReference->m_map;
	Entity *entity = map->FindEntity(m_unitToFollow);
	Entity *thisEntity = map->FindEntity(m_thisUnit);
	if (entity != nullptr && thisEntity != nullptr)
	{
		thisEntity->Follow(entity);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
AttackTask::AttackTask(const GameHandle& thisUnit, const GameHandle& unitToAttack)
	: RTSTask(ATTACK)
{
	m_thisUnit = thisUnit;
	m_unitToAttack = unitToAttack;
}

//------------------------------------------------------------------------------------------------------------------------------
AttackTask::~AttackTask()
{

}

//------------------------------------------------------------------------------------------------------------------------------
VIRTUAL void AttackTask::Execute()
{
	Map* map = Game::s_gameReference->m_map;
	Entity *entity = map->FindEntity(m_unitToAttack);
	Entity *thisEntity = map->FindEntity(m_thisUnit);
	if (entity != nullptr && thisEntity != nullptr)
	{
		thisEntity->Attack(entity);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
GatherTask::GatherTask(const GameHandle& thisUnit, const GameHandle& unitToGather)
	: RTSTask(GATHER)
{
	m_thisUnit = thisUnit;
	m_unitToGather = unitToGather;
}

//------------------------------------------------------------------------------------------------------------------------------
GatherTask::~GatherTask()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void GatherTask::Execute()
{
	Map* map = Game::s_gameReference->m_map;
	Entity *entity = map->FindEntity(m_unitToGather);
	Entity *thisEntity = map->FindEntity(m_thisUnit);
	if (entity != nullptr && thisEntity != nullptr)
	{
		thisEntity->Gather(entity);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
BuildTask::BuildTask(const GameHandle& thisUnit, const Vec2& buildLocation, EntityTypeT entityType)
	:RTSTask(BUILD)
{
	m_thisUnit = thisUnit;
	m_buildLocation = buildLocation;
	m_buildingType = entityType;
}

//------------------------------------------------------------------------------------------------------------------------------
BuildTask::~BuildTask()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void BuildTask::Execute()
{
	Map* map = Game::s_gameReference->m_map;
	Entity *thisEntity = map->FindEntity(m_thisUnit);

	if (thisEntity != nullptr)
	{
		thisEntity->Build(m_buildLocation, m_buildingType);
	}
}
