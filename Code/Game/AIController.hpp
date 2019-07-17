//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Game Systems
#include "Game/Game.hpp"
#include "Game/GameHandle.hpp"
#include "Game/GameTypes.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class AIController
{
public:
	AIController(Game* game);
	~AIController();

	void	Startup();
	
	void	CreateGoblinTownCenter();
	void	CreateTreesNearTownCenter();
	
	void	BeginFrame();
	void	Update();
	void	DebugRender();
	void	EndFrame();
	void	ShutDown();

public:
	Game*	m_game = nullptr;
	int		m_AITeam = 2;
};