#include "Game/AIController.hpp"
#include "Game/Map.hpp"
#include "Game/Entity.hpp"

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
	SpawnStartUnits();
}

//------------------------------------------------------------------------------------------------------------------------------
void AIController::CreateGoblinTownCenter()
{
	IntVec2 mapDimensions = m_game->m_map->m_tileDimensions;
	IntVec2 townCenterOcc = m_game->m_map->m_townCenterOcc;

	Vec2 spawnPos = Vec2(mapDimensions - townCenterOcc - IntVec2(2,2));
	spawnPos -= Vec2(0.5f, 0.5f);

	m_townCenterPos = spawnPos;

	m_goblinTownCenter = m_game->m_map->CreateEntity(spawnPos, TOWNCENTER, m_AITeam);
	m_goblinTownCenter->SetHealth(m_goblinTownCenter->GetMaxHealth());
}

//------------------------------------------------------------------------------------------------------------------------------
void AIController::CreateTreesNearTownCenter()
{
	IntVec2 mapDimensions = m_game->m_map->m_tileDimensions;
	IntVec2 townCenterOcc = m_game->m_map->m_townCenterOcc;

	Vec2 spawnPos = Vec2(mapDimensions - townCenterOcc - IntVec2(10, 10));
	spawnPos -= Vec2(0.5f, 0.5f);

	while (spawnPos.y < (float)mapDimensions.y - 1.f)
	{
		m_game->m_map->CreateEntity(spawnPos, TREE, 0);

		spawnPos += Vec2(0.f, 1.f);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void AIController::SpawnStartUnits()
{
	for (int unitIndex = 0; unitIndex < m_startUnitCount; unitIndex++)
	{
		Entity* entity = m_game->m_map->CreateEntity(m_townCenterPos - Vec2(0.f, unitIndex * 3.f), GOBLIN, m_AITeam);
		m_AIentities.push_back(entity);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void AIController::Update(float deltaTime)
{
	if (m_AIentities.size() < 8 && m_game->m_teamResource[m_AITeam - 1] > 50.f)
	{
		TrainUnit();
	}
	else if(m_game->m_teamResource[m_AITeam - 1] < 50.f)
	{
		for (int unitIndex = 0; unitIndex < m_AIentities.size(); unitIndex++)
		{
			Entity* closestTree = m_game->m_map->GetClosestEntityOfType(TREE, m_AIentities[unitIndex]->GetPosition());
			m_AIentities[unitIndex]->Gather(closestTree);
		}
	}
	else
	{
		if (m_game->m_teamResource[m_AITeam - 1] > 50.f)
		{
			//Attack task here
			for (int unitIndex = 0; unitIndex < m_AIentities.size(); unitIndex++)
			{
				Entity* closestPeon = m_game->m_map->GetClosestEntityOfType(PEON, m_AIentities[unitIndex]->GetPosition(), 2);
				if (closestPeon != nullptr)
				{
					m_AIentities[unitIndex]->Attack(closestPeon);
				}
				else
				{
					Entity* closestHut = m_game->m_map->GetClosestEntityOfType(HUT, m_AIentities[unitIndex]->GetPosition(), 2);
					if (closestHut != nullptr)
					{
						m_AIentities[unitIndex]->Attack(closestHut);
					}
					else
					{
						Entity* closestTownCenter = m_game->m_map->GetClosestEntityOfType(TOWNCENTER, m_AIentities[unitIndex]->GetPosition(), 2);
						if (closestTownCenter != nullptr)
						{
							m_AIentities[unitIndex]->Attack(closestTownCenter);
						}
					}
				}
			}
		}
		else
		{
			for (int unitIndex = 0; unitIndex < m_AIentities.size(); unitIndex++)
			{
				Entity* closestTree = m_game->m_map->GetClosestEntityOfType(TREE, m_AIentities[unitIndex]->GetPosition());
				m_AIentities[unitIndex]->Gather(closestTree);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void AIController::TrainUnit()
{
	if (m_game->m_teamCurrentSupply[m_AITeam - 1] >= m_game->m_teamMaxSupply[m_AITeam - 1])
		return;

	if (m_game->m_teamResource[m_AITeam - 1] >= m_game->m_map->GetGoblinCost())
	{
		m_goblinTownCenter->SetIsTrainingUnit(true);

		m_game->m_teamResource[m_AITeam - 1] -= m_game->m_map->GetGoblinCost();
		m_game->m_teamCurrentSupply[m_AITeam - 1] += 1;
	}
}