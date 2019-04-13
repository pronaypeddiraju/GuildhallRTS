//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Map.hpp"

// Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/Vertex_Lit.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderContext.hpp"

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
void Map::Update()
{
	//Do nothing for now
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::Render() const
{
	RenderTerrain(m_terrainMaterial);
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
AABB2 Map::GetXYBounds() const
{
	return m_mapBounds;
}
