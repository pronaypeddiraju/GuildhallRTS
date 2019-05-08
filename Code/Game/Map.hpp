//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>
#include <cstdint>

typedef unsigned int uint;
typedef uint16_t uint16;

//------------------------------------------------------------------------------------------------------------------------------
struct Ray3D;
struct Vertex_Lit;
class Entity;
class GameHandle;
class GPUMesh;
class Material;

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
	bool				Load( char const* filename );          
	bool				Create( int mapWidth, int mapHeight ); 

	void				Update(); 
	void				UpdateEntities(); 
	void				Render() const; // assumes a camera is already bound
	void				Shutdown();

	void				RenderTerrain( Material* matOverride = nullptr ) const;

	// Accessors
	AABB2				GetXYBounds() const; // used for constraining the camera's focal point

	// Entity Methods
	Entity*				CreateEntity(const Vec2& pos);
	Entity*				FindEntity(const GameHandle& handle) const;

	// Pick
	Entity*				RaycastEntity(float *out, const Ray3D& ray, float maxDistance = INFINITY);
	uint				RaycastTerrain(float* out, const Ray3D& ray);

private:
	void				PurgeDestroyedEntities();   // cleanup destroyed entities, freeing up the slots; 

	uint				GetFreeEntityIndex(); // return a free entity slot
	uint				GetNextCyclicID();     // gets the next hi-word to use, skipping '0'

public: 
	IntVec2					m_tileDimensions; // how many tiles X and Y
	IntVec2					m_vertDimensions; // how many verts X and Y

private:
	std::vector<MapTile>	m_mapTiles;
	std::vector<Vertex_Lit> m_mapVerts; 
	std::vector<uint>		m_mapIndices;

	std::string				m_materialName = "terrain.mat";

	GPUMesh*				m_terrainMesh = nullptr; 
	Material*				m_terrainMaterial = nullptr; 

	AABB2					m_mapBounds;

	// map entity data
	std::vector<Entity*>	m_entities;
	uint16					m_cyclicID = 0; // used for generating the GameHandle
};
