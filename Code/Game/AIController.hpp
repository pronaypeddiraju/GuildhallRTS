//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Game Systems
#include "Game/Game.hpp"
#include "Game/GameHandle.hpp"
#include "Game/GameTypes.hpp"

class Entity;

//------------------------------------------------------------------------------------------------------------------------------
class AIController
{
public:
	AIController(Game* game);
	~AIController();

	void	Startup();
	
	void	CreateGoblinTownCenter();
	void	CreateTreesNearTownCenter();
	void	SpawnStartUnits();
	
	void	Update(float deltaTime);
	void	TrainUnit();


public:
	Game*	m_game = nullptr;
	int		m_AITeam = 2;
	int		m_startUnitCount = 2;

	Vec2	m_townCenterPos = Vec2::ZERO;
	Entity*	m_goblinTownCenter = nullptr;

	std::vector<Entity*>	m_AIentities;
};