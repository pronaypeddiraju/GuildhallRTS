//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>

typedef unsigned int uint;

//------------------------------------------------------------------------------------------------------------------------------
struct Vertex_Lit;
class Material;
class GPUMesh;

//------------------------------------------------------------------------------------------------------------------------------
struct MapTile
{
	// empty for now; 
	int m_placeholder; 
}; 

//------------------------------------------------------------------------------------------------------------------------------
class Map
{
public:
	Map();
	~Map();

	//For now load just calls create with 64x64 as parameters
	bool Load( char const* filename );          
	bool Create( int mapWidth, int mapHeight ); 

	void Update(); 
	void Render() const; // assumes a camera is already bound
	void Shutdown();

	void RenderTerrain( Material* matOverride = nullptr ) const;

	// Accessors
	AABB2 GetXYBounds() const; // used for constraining the camera's focal point

private:
	void GenerateTerrainMesh(); // creates the mesh and material from the tiles; 

public: 
	IntVec2					m_tileDimensions; // how many tiles X and Y
	IntVec2					m_vertDimensions; // how many verts X and Y

private:
	std::vector<MapTile>	m_mapTiles;
	std::vector<Vertex_Lit> m_mapVerts; 
	std::vector<uint>		m_mapIndices;

	std::string				m_materialName = "terrain.mat";

	GPUMesh*				m_terrainMesh; 
	Material*				m_terrainMaterial; 

	AABB2					m_mapBounds;
};
