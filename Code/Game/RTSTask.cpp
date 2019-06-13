#include "Game/RTSTask.hpp"
#include "Engine/Commons/EngineCommon.hpp"
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
