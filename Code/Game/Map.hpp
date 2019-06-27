//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/AnimTypes.hpp"
#include <vector>
#include <map>
#include <cstdint>

#include "Game/GameTypes.hpp"

typedef unsigned int uint;
typedef uint16_t uint16;

//------------------------------------------------------------------------------------------------------------------------------
struct Frustum;
struct Ray3D;
struct Rgba;
struct Vertex_Lit;
class Entity;
class GameHandle;
class GPUMesh;
class CPUMesh;
class Material;
class SpriteDefenition;
class IsoSpriteDefenition;
class GPUMesh;
class RTSCamera;
class SpriteSheet;
class Model;

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
	void				LoadFoliageModels();
	void				LoadBuildingModels();
	bool				Create(int mapWidth, int mapHeight);

	void				Update(float deltaTime); 
	void				UpdateEntities(float deltaTime);
	void				ClearDeadEntities();
	void				Render() const; // assumes a camera is already bound
	void				Shutdown();

	void				RenderTerrain( Material* matOverride = nullptr ) const;
	void				RenderEntities() const;
	void				RenderEntityData() const;
	void				RenderIsoSpriteForEntity(const Entity& entity) const;
	void				DrawHealthBar(const Entity& entity) const;
	void				RenderResourceEntity(const Entity& entity) const;
	void				RenderTownCenter(const Entity& entity) const;
	void				RenderBuildingPreview() const;
	void				DrawBillBoardedIsoSprites(const Vec2& position, const Vec3& orientation, const IsoSpriteDefenition& isoDef, const RTSCamera& camera, EntityTypeT type, const Rgba& drawColor, eAnimationType animState) const;
	void				DrawBillBoardedSprite(const Vec3& position, const SpriteDefenition& sprite, const RTSCamera& camera, EntityTypeT type, const Rgba& drawColor, eAnimationType animState) const;

	// Accessors
	AABB2				GetXYBounds() const; // used for constraining the camera's focal point

	// Entity Methods
	Entity*				CreateEntity(const Vec2& pos, EntityTypeT entityType, int team = 1);
	Entity*				FindEntity(const GameHandle& handle) const;
	Entity*				GetEntityAtIndex(int index);
	void				ResolveEntityCollisions();

	// Pick
	Entity*				RaycastEntity(float *out, const Ray3D& ray, float maxDistance = INFINITY);
	uint				RaycastTerrain(float* out, const Ray3D& ray);
	void				SelectEntitiesInFrustum(std::vector<GameHandle>& entityHandles, const Frustum& selectionFrustum);
	bool				IsEntitySelected(const Entity& entity) const;

	int					GetNumEntities();

private:
	void				PurgeDestroyedEntities();   // cleanup destroyed entities, freeing up the slots; 

	uint				GetFreeEntityIndex(); // return a free entity slot
	uint				GetNextCyclicID();     // gets the next hi-word to use, skipping '0'

public: 
	IntVec2					m_tileDimensions; // how many tiles X and Y
	IntVec2					m_vertDimensions; // how many verts X and Y
	IntVec2					m_townCenterOcc = IntVec2(4, 3);

private:
	std::vector<MapTile>	m_mapTiles;
	std::vector<Vertex_Lit> m_mapVerts; 
	std::vector<uint>		m_mapIndices;
	std::map<int, bool>		m_mapOccupancy;

	std::string				m_materialName = "terrain.mat";
	
	GPUMesh*				m_terrainMesh = nullptr; 
	Material*				m_terrainMaterial = nullptr; 

	CPUMesh*				m_entityCPUMesh = nullptr;
	GPUMesh*				m_entityMesh = nullptr; 
	
	//Building Models
	Model*					m_townCenter = nullptr;

	AABB2					m_mapBounds;

	// map entity data
	std::vector<Entity*>	m_entities;
	uint16					m_cyclicID = 0; // used for generating the GameHandle

	//Entity Draw Data
	float					m_entityWidth = 3.f;
	float					m_entityHeight = 3.f;
	float					m_entitySelectRadius = 1.f;
	float					m_entitySelectWidth = 0.1f;

	//Heath bar data
	float					m_healthBarWidth = 1.f;
	float					m_healthBarHeight = 0.1f;
	Vec2					m_healthBarPivot = Vec2(0.5, -2.0f);

	GPUMesh*				m_quad = nullptr;

	//Data driving 
	std::string				m_peonXMLFile = "Data/Gameplay/peon.xml";
	std::string				m_warriorXMLFile = "Data/Gameplay/warrior.xml";
	std::string				m_treeXMLFile = "Data/Gameplay/tree.xml";
	std::string				m_townCenterXMLFile = "Data/Gameplay/building_townCenter.xml";

	std::string				m_treeModelsXMLFile = "Data/Gameplay/tree_models.xml";
	std::string				m_buildingModelsXMLFile = "Data/Gameplay/building_models.xml";
	std::string				m_treeMaterialFile = "Data/Models/foliage/foliage.mat";
	std::string				m_townCenterModelFile = "building/towncenter.mesh";
	std::string				m_townCenterMaterial = "building/towncenter.mat";
};
