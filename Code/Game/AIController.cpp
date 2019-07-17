#include "Game/AIController.hpp"
#include "Game/Map.hpp"

//------------------------------------------------------------------------------------------------------------------------------
AIController::AIController(Game* game)
{
	m_game = game;
}

//------------------------------------------------------------------------------------------------------------------------------
AIController::~AIController()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void AIController::Startup()
{
	CreateGoblinTownCenter();
	CreateTreesNearTownCenter();
}

//------------------------------------------------------------------------------------------------------------------------------
void AIController::CreateGoblinTownCenter()
{
	IntVec2 mapDimensions = m_game->m_map->m_tileDimensions;
	IntVec2 townCenterOcc = m_game->m_map->m_townCenterOcc;

	Vec2 spawnPos = Vec2(mapDimensions - townCenterOcc - IntVec2(2,2));
	spawnPos -= Vec2(0.5f, 0.5f);

	m_game->m_map->CreateEntity(spawnPos, TOWNCENTER, m_AITeam);
}

//------------------------------------------------------------------------------------------------------------------------------
void AIController::CreateTreesNearTownCenter()
{
	IntVec2 mapDimensions = m_game->m_map->m_tileDimensions;
	IntVec2 townCenterOcc = m_game->m_map->m_townCenterOcc;

	Vec2 spawnPos = Vec2(mapDimensions - townCenterOcc - IntVec2(10, 10));
	spawnPos -= Vec2(0.5f, 0.5f);

	while (spawnPos.y < (float)mapDimensions.y)
	{
		m_game->m_map->CreateEntity(spawnPos, TREE, 0);

		spawnPos += Vec2(1.f, 1.f);
	}
}