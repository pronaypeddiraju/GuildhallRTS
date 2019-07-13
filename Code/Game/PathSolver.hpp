//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Array2D.hpp"
#include <vector>

enum ePathState
{
	PATH_STATE_UNVISITED = 0,
	PATH_STATE_VISITED,
	PATH_STATE_FINISHED,
};

struct PathInfo_T
{
	float cost = INFINITY;  // how much did it cost to reach this point
	IntVec2 tile = IntVec2(-1, -1); // who was I visited by
	ePathState state = PATH_STATE_UNVISITED;

	bool	operator==(const PathInfo_T& compare) const;
};

typedef Array2D<float> TileCosts;
typedef Array2D<PathInfo_T> PathInfo;
typedef std::vector<IntVec2> Path;

//------------------------------------------------------------------------------------------------------------------------------
// This object will keep all the costs on the map tiles and create paths for us
//------------------------------------------------------------------------------------------------------------------------------
class Pather
{
public:
	void		Init(const IntVec2& mapSize, float initialCost);

	void		SetAllCosts(float cost);
	void		SetCost(const IntVec2& cell, float cost);
	void		AddCost(const IntVec2& cell, float costToAdd);

public:
	TileCosts	m_costs;
};

//------------------------------------------------------------------------------------------------------------------------------
// This object will use the Pather and determine what path to take (i.e: This is the object that runs Dijkstra's algorithm)
//------------------------------------------------------------------------------------------------------------------------------
class PathSolver
{
public:
	//The function that actually takes a pather and does the distance field calculations
	//You need to set a seed point (the end we set) and calculate Distance Field from it
	void		StartDistanceField(Pather* pather);

	PathInfo_T	PopLowestCostCellFromList(std::vector<PathInfo_T>& openList);
	void		PushToOpenList(const std::vector<PathInfo_T>& neighbors, std::vector<PathInfo_T>& openList);
	
	void		GetNeighbors(const PathInfo_T& cell, std::vector<PathInfo_T>& neighbors);
	void		GetNeighborsFromVisited(const PathInfo_T& cell, std::vector<PathInfo_T>& neighbors);
	void		GetCheapestNeighbors(std::vector<PathInfo_T>& cheapestCostCells, const std::vector<PathInfo_T>& list);

	void		SetNeighbor(const PathInfo_T neighbor, std::vector<PathInfo_T>& neighborsArray, int index);
	void		SetNeighborCosts(std::vector<PathInfo_T>& neighborsArray, float previousTileCost);
	
	void		AddEnd(const IntVec2& tile);		//We will flood fill from this destination
	void		AddStart(const IntVec2& tile);		//Technically becomes our end point for Dijkstra

	void		FallDownToShortestPath(Path& shortestPath);
	void		RemoveNeighborsIfInList(std::vector<PathInfo_T>& neighbors, std::vector<IntVec2>& shortestPath);

private:

	Pather*						m_pather = nullptr;
	std::vector<PathInfo_T>		m_visited;
	std::vector<IntVec2>		m_termination_points; // used for ending early if you have a start point in mind; 
	PathInfo					m_pathInfo;

	IntVec2			m_endPoint = IntVec2(-1, -1);	//Start of flood fill
	IntVec2			m_startPoint = IntVec2(-1, -1);

	Path*			m_shortestPath = nullptr;
};