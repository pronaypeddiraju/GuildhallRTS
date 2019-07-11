//------------------------------------------------------------------------------------------------------------------------------
#include "Game/PathSolver.hpp"

//------------------------------------------------------------------------------------------------------------------------------
void Pather::Init(const IntVec2& mapSize, float initialCost)
{
	m_costs.Init(mapSize, initialCost);
}

//------------------------------------------------------------------------------------------------------------------------------
void Pather::SetAllCosts(float cost)
{
	m_costs.SetAll(cost);
}

//------------------------------------------------------------------------------------------------------------------------------
void Pather::SetCost(const IntVec2& cell, float cost)
{
	m_costs.Set(cell, cost);
}

//------------------------------------------------------------------------------------------------------------------------------
void Pather::AddCost(const IntVec2& cell, float costToAdd)
{
	float currentCost = m_costs.Get(cell);
	float newCost = currentCost + costToAdd;
	m_costs.Set(cell, newCost);
}

