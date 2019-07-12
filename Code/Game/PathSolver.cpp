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
	IntVec2 bounds = m_costs.GetSize();
	if (cell.IsInBounds(bounds))
		return;

	m_costs.Set(cell, cost);
}

//------------------------------------------------------------------------------------------------------------------------------
void Pather::AddCost(const IntVec2& cell, float costToAdd)
{
	float currentCost = m_costs.Get(cell);
	float newCost = currentCost + costToAdd;
	m_costs.Set(cell, newCost);
}

//------------------------------------------------------------------------------------------------------------------------------
// Method to create the distance field based on costs
//------------------------------------------------------------------------------------------------------------------------------
void PathSolver::StartDistanceField(Pather* pather)
{
	m_visited.clear();
	m_pather = pather;
	m_shortestPath = new Path;
	PathInfo_T info;
	m_pathInfo.Init(m_pather->m_costs.GetSize(), info);

	//Make array for neighbor tiles and an openList vector
	std::vector<PathInfo_T> neighbors(4);
	std::vector<PathInfo_T> openList;

	//We are visiting the first point by default so put that into the m_visited and update m_pathInfo
	//info.cost = 0.f;
	//info.state = PATH_STATE_VISITED;
	//m_pathInfo.Set(m_endPoint, info);
	//m_visited.push_back(m_endPoint);

	//m_shortestPath->push_back(m_endPoint);
	info.cost = m_pather->m_costs.Get(m_endPoint);
	info.tile = m_endPoint;
	openList.push_back(info);

	//Now we need to visit each neighbor
	//GetNeighbors(m_endPoint, neighbors);
	//PushToOpenList(neighbors, openList);

	while (!openList.empty())
	{
		//Get cell with minimum cost and delete from set
		PathInfo_T lowestCostCell = PopLowestCostCellFromOpenList(openList);

		//Visit the lowest cost cell
		m_visited.push_back(lowestCostCell);
		info.cost = m_pather->m_costs.Get(lowestCostCell.tile);
		info.state = PATH_STATE_VISITED;
		m_pathInfo.Set(lowestCostCell.tile, info);

		//Get neighbors and push into open list
		GetNeighbors(lowestCostCell, neighbors);
		PushToOpenList(neighbors, openList);

		m_shortestPath->push_back(lowestCostCell.tile);
	}
	
	//Fall down to the end spot

}

//------------------------------------------------------------------------------------------------------------------------------
PathInfo_T PathSolver::PopLowestCostCellFromOpenList(std::vector<PathInfo_T>& openList)
{
	float lowestCost = 10000.f;
	PathInfo_T lowestCostCell;
	lowestCostCell.tile = IntVec2(-1, -1);
	int lowestIndex = 0;
	for (int index = 0; index < openList.size(); index++)
	{
		if (m_pather->m_costs.Get(openList[index].tile) < lowestCost)
		{
			lowestCost = m_pather->m_costs.Get(openList[index].tile);
			lowestCostCell = openList[index];
			lowestIndex = index;
		}
	}

	openList.erase(openList.begin() + lowestIndex);
	return lowestCostCell;
}

//------------------------------------------------------------------------------------------------------------------------------
void PathSolver::PushToOpenList(const std::vector<PathInfo_T>& neighbors, std::vector<PathInfo_T>& openList)
{
	for (int index = 0; index < 4; index++)
	{
		if (neighbors[index].tile != IntVec2(-1, -1))
		{			
			if (std::find(openList.begin(), openList.end(), neighbors[index]) == openList.end())
			{
				if (m_pathInfo.Get(neighbors[index].tile).state == PATH_STATE_UNVISITED)
				{
					openList.push_back(neighbors[index]);
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void PathSolver::GetNeighbors(const PathInfo_T& cell, std::vector<PathInfo_T>& neighbors)
{
	PathInfo_T neighbor;
	neighbor.tile = cell.tile;

	//Left neighbor
	neighbor.tile.x -= 1;
	neighbor.cost = m_pather->m_costs.Get(cell.tile);
	SetNeighbor(neighbor, neighbors, 0);

	//Right neighbor
	neighbor = cell;
	neighbor.tile.x += 1;
	neighbor.cost = m_pather->m_costs.Get(cell.tile);
	SetNeighbor(neighbor, neighbors, 1);

	//Top neighbor
	neighbor = cell;
	neighbor.tile.y += 1;
	neighbor.cost = m_pather->m_costs.Get(cell.tile);
	SetNeighbor(neighbor, neighbors, 2);

	//Bottom neighbor
	neighbor = cell;
	neighbor.tile.y -= 1;
	neighbor.cost = m_pather->m_costs.Get(cell.tile);
	SetNeighbor(neighbor, neighbors, 3);
}

//------------------------------------------------------------------------------------------------------------------------------
void PathSolver::SetNeighbor(const PathInfo_T neighbor, std::vector<PathInfo_T>& neighborsArray, int index)
{
	if (neighbor.tile.IsInBounds(m_pather->m_costs.GetSize()))
	{
		neighborsArray[index] = neighbor;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void PathSolver::AddEnd(const IntVec2& tile)
{
	m_endPoint = tile;
}

//------------------------------------------------------------------------------------------------------------------------------
void PathSolver::AddStart(const IntVec2& tile)
{
	m_startPoint = tile;
}

//------------------------------------------------------------------------------------------------------------------------------
bool PathInfo_T::operator==(const PathInfo_T& compare) const
{
	if (cost == compare.cost && tile == compare.tile && state == compare.state)
	{
		return true;
	}
	else
	{
		return false;
	}
}
