//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Map.hpp"

// Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/Frustum.hpp"
#include "Engine/Math/Plane3D.hpp"
#include "Engine/Math/Ray3D.hpp"
#include "Engine/Math/Vertex_Lit.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderContext.hpp"
//Game Systems
#include "Game/GameHandle.hpp"
#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#include "Game/GameInput.hpp"

extern RenderContext* g_renderContext;

//------------------------------------------------------------------------------------------------------------------------------
Map::Map()
{
	//for tier 1 we will be doing absolutely fuck all here :)
}

//------------------------------------------------------------------------------------------------------------------------------
Map::~Map()
{
	Shutdown();
}

//------------------------------------------------------------------------------------------------------------------------------
// Will eventually load up saved maps using filename. for now simply call create with 64x64
//------------------------------------------------------------------------------------------------------------------------------
bool Map::Load( char const* filename )
{
	UNUSED(filename);

	m_terrainMaterial = g_renderContext->CreateOrGetMaterialFromFile(m_materialName);

	bool result = Create(64, 64);
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------
/** For a 2x2 map, the vertices will look like (each + is a unique vertex)
This will give us more painting fidelity, and visual fidelity if we choose to futz with 
height

	+---+---+---+---+
	| / | / | / | / |
1	+---+---+---+---+
	| / | / | / | / |
	+---+---+---+---+
	| / | / | / | / |
0	+---+---+---+---+
	| / | / | / | / |
^	+---+---+---+---+
y		0       1
x->


The map will be setup such that X -> Right, Y -> Up.  So that we can keep with a left handed basis and not deal 
with the change of basis going into the rendering system, we'll just assume Z -> down (instead of the more intuitive Z -> up)

Each tile is 1 unit length in the world (so each quad is half length)
Each tile is one UV of texture (so each quad is half a uv) - be sure to use a wrapping sampler for terrain for this; 

**/
//------------------------------------------------------------------------------------------------------------------------------
bool Map::Create( int mapWidth, int mapHeight )
{
	//Create a map grid
	m_tileDimensions = IntVec2(mapWidth, mapHeight);
	
	//Create a temp CPU Mesh for now
	CPUMesh mesh;
	mesh.Clear();

	int vertsX = 2 * mapWidth + 1;
	int vertsY = 2 * mapHeight + 1;

	float u = 0.f;
	float v = (float)vertsY;

	mesh.SetLayout<Vertex_Lit>();
	mesh.SetColor(Rgba::WHITE);
	mesh.SetNormal(Vec3(0.f, 0.f, -1.f));
	mesh.SetTangent(Vec3(1.f, 0.f, 0.f));
	mesh.SetBiTangent(Vec3(0.f, 1.f, 0.f));
	
	for(int yIndex = 0; yIndex < vertsY; ++yIndex)
	{
		for(int xIndex = 0; xIndex < vertsX; ++xIndex)
		{
			Vertex_Lit vert;
			vert.m_position = Vec3(xIndex * 0.5f - 0.5f, yIndex * 0.5f - 0.5f, 0.f);
			vert.m_uv = Vec2(u, v);

			//Setup data on the CPUMesh
			mesh.SetUV(Vec2(u,v));
			mesh.AddVertex(vert.m_position);

			//Push into vector for map
			m_mapVerts.push_back(vert);
			u += 0.5f;
		}
		u = 0.f;
		v -= 0.5f;
	}

	for(int yIndex = 0; yIndex < vertsY - 1; ++yIndex)
	{
		for(int xIndex = 0; xIndex < vertsX - 1; ++xIndex)
		{
			int botLeft = xIndex + yIndex * vertsX;
			int botright = botLeft + 1;
			int topLeft = botLeft + vertsX;
			int topRight = topLeft + 1;

			//Push into vector for map
			m_mapIndices.push_back(botLeft);
			m_mapIndices.push_back(botright);
			m_mapIndices.push_back(topRight);

			m_mapIndices.push_back(botLeft);
			m_mapIndices.push_back(topRight);
			m_mapIndices.push_back(topLeft);

			//Add index to the mesh
			mesh.AddIndexedQuad(topLeft, topRight, botLeft, botright);
		}
	}

	if(m_terrainMesh != nullptr)
	{
		delete m_terrainMesh;
		m_terrainMesh = nullptr;
	}

	//Copy the CPU mesh info to the GPUMesh
	m_terrainMesh = new GPUMesh(g_renderContext);
	m_terrainMesh->CreateFromCPUMesh<Vertex_Lit>(&mesh, GPU_MEMORY_USAGE_STATIC);

	//Set the map bounds in the AABB2
	m_mapBounds = AABB2(Vec2(m_mapVerts[0].m_position.x, m_mapVerts[0].m_position.y), Vec2(m_mapVerts[(int)m_mapVerts.size() - 1].m_position.x, m_mapVerts[(int)m_mapVerts.size() - 1].m_position.y));

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::Update(float deltaTime)
{
	UpdateEntities(deltaTime);
}

void Map::UpdateEntities(float deltaTime)
{
	int numEntities = (int)m_entities.size();
	for (int index = 0; index < numEntities; index++)
	{
		m_entities[index]->Update(deltaTime);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::Render() const
{
	RenderTerrain(m_terrainMaterial);
	RenderEntities();
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::Shutdown()
{
	delete m_terrainMesh;
	m_terrainMesh = nullptr;

	//Empty both the vectors we have of verts and of MapTiles
	m_mapTiles.clear();
	m_mapVerts.clear();
	m_mapIndices.clear();
}

void Map::RenderTerrain( Material* matOverride /*= nullptr */ ) const
{
	if(matOverride != nullptr)
	{
		g_renderContext->BindMaterial(matOverride);
	}
	else
	{
		g_renderContext->BindMaterial(m_terrainMaterial);
	}
	g_renderContext->DrawMesh(m_terrainMesh);
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::RenderEntities() const
{
	int numEntities = (int)m_entities.size();
	for (int index = 0; index < numEntities; index++)
	{
		g_renderContext->BindShader(g_renderContext->CreateOrGetShaderFromFile("default_lit.hlsl"));
		g_renderContext->BindTextureView(0U, nullptr);

		CPUMesh mesh;
		Capsule3D capsule;
		capsule = m_entities[index]->CreateEntityCapsule();

		if (IsEntitySelected(*m_entities[index]))
		{
			//CPUMeshAddUVCapsule(&mesh, capsule.m_start, capsule.m_end, capsule.m_radius, Rgba::YELLOW);
			CPUMeshAddUVCapsule(&mesh, Vec3(0.f, 1.f, 0.f), Vec3::ZERO, capsule.m_radius, Rgba::YELLOW);
		}
		else
		{
			//CPUMeshAddUVCapsule(&mesh, capsule.m_start, capsule.m_end, capsule.m_radius, Rgba::WHITE);
			CPUMeshAddUVCapsule(&mesh, Vec3(0.f, 1.f, 0.f), Vec3::ZERO, capsule.m_radius, Rgba::WHITE);
		}

		GPUMesh drawMesh = GPUMesh(g_renderContext);
		drawMesh.CreateFromCPUMesh<Vertex_Lit>(&mesh);

		//Setup the model matrix for the entity
		Matrix44 mat = Matrix44::MakeXRotationDegrees(90.f);
		Matrix44 translation = Matrix44::MakeTranslation3D(capsule.m_end);
		Vec3 t = translation.GetTVector();
		mat.SetTVector(t);
		g_renderContext->BindModelMatrix(mat);

		//Draw in wireframe
		g_renderContext->SetRasterStateWireFrame();
		g_renderContext->DrawMesh(&drawMesh);
		g_renderContext->CreateAndSetDefaultRasterState();
	}
}

bool Map::IsEntitySelected(const Entity& entity) const
{
	GameInput* inputClass = Game::s_gameReference->m_gameInput;

	for (int i = 0; i < (int)inputClass->m_selectionHandles.size(); ++i)
	{
		if (entity.GetHandle() == inputClass->m_selectionHandles[i])
		{
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------------------------
AABB2 Map::GetXYBounds() const
{
	return m_mapBounds;
}

//------------------------------------------------------------------------------------------------------------------------------
Entity* Map::CreateEntity(const Vec2& pos)
{
	uint slot = GetFreeEntityIndex();
	uint cyclicID = GetNextCyclicID();

	GameHandle handle = GameHandle(cyclicID, slot);
	Entity *entity = new Entity(handle, pos);
	//entity->m_handle = handle;

	// you may have to grow this vector...
	m_entities[slot] = entity;
	return entity;
}

//------------------------------------------------------------------------------------------------------------------------------
Entity* Map::FindEntity(const GameHandle& handle) const
{
	uint slot = handle.GetIndex();
	Entity *entity = m_entities[slot];

	// we only return the entity if it matches the handle
	if ((entity != nullptr) && (entity->GetHandle() == handle))
	{
		return entity;
	}
	else 
	{
		return nullptr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
Entity* Map::GetEntityAtIndex(int index)
{
	if (index >= (int)m_entities.size())
	{
		return nullptr;
	}
	else
	{
		return m_entities[index];
	}
}

//------------------------------------------------------------------------------------------------------------------------------
Entity* Map::RaycastEntity(float *out, const Ray3D& ray, float maxDistance /*= INFINITY*/)
{
	Entity *bestEntity = nullptr;
	float bestTime = INFINITY;

	int numEntities = (int)m_entities.size();
	for(int index = 0; index < numEntities; index++)
	{
		Entity* entity = m_entities[index];
		float time[2];

		if (entity->IsSelectable() && entity->RaycastHit(time, ray))
		{
			float smaller = GetLowerValue(time[0], time[1]);

			if ((smaller >= 0.0f) && (smaller <= maxDistance) && (smaller < bestTime))
			{
				bestEntity = entity;
				bestTime = smaller;
			}
		}
	}

	Vec3 point = ray.m_start + (ray.m_direction * bestTime);
	g_debugRenderer->DebugRenderLine(ray.m_start, point, 1.f);

	*out = bestTime;
	return bestEntity;
}

//------------------------------------------------------------------------------------------------------------------------------
uint Map::RaycastTerrain(float* out, const Ray3D& ray)
{
	Plane3D terrainPlane(Vec3(0.f, 0.f, -1.f), 0.f);
	return Raycast(out, ray, terrainPlane);
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::SelectEntitiesInFrustum(std::vector<GameHandle>& entityHandles, const Frustum& selectionFrustum)
{
	entityHandles.clear();

	//if entity is inside the frustum, set it as selected
	for (int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++)
	{
		Entity* entity = m_entities[entityIndex];
		if (selectionFrustum.ContainsPoint(entity->GetPosition()))
		{
			if (entity->IsSelectable() && entity->GetHandle() != GameHandle::INVALID)
			{
				entity->SetSelectable(true);
				entityHandles.push_back(entity->GetHandle());
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
int Map::GetNumEntities()
{
	return (int)m_entities.size();
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::PurgeDestroyedEntities()
{
	for (int index = 0; index < (int)m_entities.size(); index++)
	{
		if (m_entities[index]->IsDestroyed())
		{
			delete m_entities[index];
			index++;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
uint Map::GetFreeEntityIndex()
{
	//Find the first empty slot for an entity in the game
	for (int index = 0; index < (int)m_entities.size(); index++)
	{
		if (m_entities[index] == nullptr)
		{
			return index;
		}
	}

	m_entities.resize(m_entities.size() + 1);
	return (uint)m_entities.size() - 1U;
}

//------------------------------------------------------------------------------------------------------------------------------
uint Map::GetNextCyclicID()
{
	m_cyclicID = m_cyclicID++;
	return m_cyclicID;
}
