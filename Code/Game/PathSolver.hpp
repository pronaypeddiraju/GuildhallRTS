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
	IntVec2 parent = IntVec2(-1, -1); // who was I visited by
	ePathState state = PATH_STATE_UNVISITED;
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

	Path*		AllocatePath(const IntVec2& start, const IntVec2& end);
	Path*		AllocatePath(const IntVec2& start, const std::vector<IntVec2>& ends);
	void		FreePath(Path* path);

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

	void		AddEnd(const IntVec2& tile);		//We will flood fill from this destination
	void		AddStart(const IntVec2& tile);		//Technically becomes our end point for Djikstra

	bool		Step();		
	void		Solve();	//run all steps and get path

	bool		GetPath(Path* out, const IntVec2& tile);

private:
	void		VisitDijkstra(const IntVec2& cell, const IntVec2& parent, float parentCost);

private:

	Pather* m_pather = nullptr;
	std::vector<IntVec2> m_visited;
	std::vector<IntVec2> m_termination_points; // used for ending early if you have a start point in mind; 
	PathInfo m_pathInfo;
};