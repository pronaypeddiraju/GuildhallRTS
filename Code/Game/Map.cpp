//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Map.hpp"

// Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Frustum.hpp"
#include "Engine/Math/Plane3D.hpp"
#include "Engine/Math/Ray3D.hpp"
#include "Engine/Math/Vertex_Lit.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Model.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteDefenition.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Shader.hpp"

//Game Systems
#include "Game/GameHandle.hpp"
#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#include "Game/GameInput.hpp"
#include "Game/RTSCamera.hpp"
#include "Game/IsoAnimDefenition.hpp"

extern RenderContext* g_renderContext;

//------------------------------------------------------------------------------------------------------------------------------
Map::Map()
{
	m_quad = new GPUMesh(g_renderContext);
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
	m_townCenter = Game::s_gameReference->m_initMesh;

	m_redShader = g_renderContext->CreateOrGetShaderFromFile(m_redShaderPath);

	LoadFoliageModels();
	LoadBuildingModels();

	bool result = Create(64, 64);
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::LoadFoliageModels()
{
	//Open the xml file and parse it
	tinyxml2::XMLDocument meshDoc;
	meshDoc.LoadFile(m_treeModelsXMLFile.c_str());

	if (meshDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{

		ERROR_AND_DIE(">> Error loading Mesh XML file ");
		return;
	}
	else
	{
		//We loaded the file successfully
		XMLElement* root = meshDoc.RootElement();
		XMLElement* childElement = root->FirstChildElement();

		std::string sourceName = "";

		while (childElement != nullptr)
		{
			sourceName = ParseXmlAttribute(*childElement, "path", "");

			if (sourceName != "")
			{
				g_renderContext->CreateOrGetMeshFromFile(sourceName);
				childElement = childElement->NextSiblingElement();
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::LoadBuildingModels()
 {
	//Open the xml file and parse it
	tinyxml2::XMLDocument meshDoc;
	meshDoc.LoadFile(m_buildingModelsXMLFile.c_str());

	if (meshDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{

		ERROR_AND_DIE(">> Error loading Mesh XML file ");
		return;
	}
	else
	{
		//We loaded the file successfully
		XMLElement* root = meshDoc.RootElement();
		XMLElement* childElement = root->FirstChildElement();

		std::string sourceName = "";

		while (childElement != nullptr)
		{
			sourceName = ParseXmlAttribute(*childElement, "path", "");

			if (sourceName != "")
			{
				g_renderContext->CreateOrGetMeshFromFile(sourceName);
				childElement = childElement->NextSiblingElement();
			}
		}
	}
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
bool Map::Create(int mapWidth, int mapHeight)
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

	for (int yIndex = 0; yIndex < vertsY; ++yIndex)
	{
		for (int xIndex = 0; xIndex < vertsX; ++xIndex)
		{
			Vertex_Lit vert;
			vert.m_position = Vec3(xIndex * 0.5f - 0.5f, yIndex * 0.5f - 0.5f, 0.f);
			vert.m_uv = Vec2(u, v);

			//Setup data on the CPUMesh
			mesh.SetUV(Vec2(u, v));
			mesh.AddVertex(vert.m_position);

			//Push into vector for map
			m_mapVerts.push_back(vert);
			u += 0.5f;
		}
		u = 0.f;
		v -= 0.5f;
	}

	for (int yIndex = 0; yIndex < vertsY - 1; ++yIndex)
	{
		for (int xIndex = 0; xIndex < vertsX - 1; ++xIndex)
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

	if (m_terrainMesh != nullptr)
	{
		delete m_terrainMesh;
		m_terrainMesh = nullptr;
	}

	//Copy the CPU mesh info to the GPUMesh
	m_terrainMesh = new GPUMesh(g_renderContext);
	m_terrainMesh->CreateFromCPUMesh<Vertex_Lit>(&mesh, GPU_MEMORY_USAGE_STATIC);

	//Set the map bounds in the AABB2
	m_mapBounds = AABB2(Vec2(m_mapVerts[0].m_position.x, m_mapVerts[0].m_position.y), Vec2(m_mapVerts[(int)m_mapVerts.size() - 1].m_position.x, m_mapVerts[(int)m_mapVerts.size() - 1].m_position.y));

	//Set occupancy values (All are false at start)
	for (int occIndex = 0; occIndex < mapHeight * mapWidth; occIndex++)
	{
		m_mapOccupancy[occIndex] = false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::Update(float deltaTime)
{
	UpdateEntities(deltaTime);

	ResolveEntityCollisions();

	ClearDeadEntities();
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::UpdateEntities(float deltaTime)
{
	int numEntities = (int)m_entities.size();
	for (int index = 0; index < numEntities; index++)
	{
		if (m_entities[index] != nullptr)
		{
			m_entities[index]->Update(deltaTime);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::ClearDeadEntities()
{
	//Remove anything that has been destroyed
	int numEntities = (int)m_entities.size();
	for (int index = 0; index < numEntities; index++)
	{
		if(m_entities[index] == nullptr)
			continue;

		if (m_entities[index]->IsGarbage())
		{
			delete m_entities[index];
			m_entities[index] = nullptr;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::Render() const
{
	RenderTerrain(m_terrainMaterial);
	//RenderEntities();
	RenderEntityData();

	GameInput* controller = Game::s_gameReference->m_gameInput;
	if ( controller->m_buildingSpawnSelect)
	{
		//Show building preview
		RenderBuildingPreview();
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::Shutdown()
{
	delete m_terrainMesh;
	m_terrainMesh = nullptr;

	delete m_quad;
	m_quad = nullptr;

	//Empty both the vectors we have of verts and of MapTiles
	m_mapTiles.clear();
	m_mapVerts.clear();
	m_mapIndices.clear();
}

//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
void Map::RenderEntityData() const
{
	//First draw a ring under any selected entity
	g_renderContext->BindShader(Game::s_gameReference->m_shader);
	g_renderContext->BindTextureView(0U, nullptr);

	GameInput* inputClass = Game::s_gameReference->m_gameInput;
	for (int index = 0; index < (int)inputClass->m_selectionHandles.size(); index++)
	{
		Entity *selected = FindEntity(inputClass->m_selectionHandles[index]);
		if (selected != nullptr)
		{
			std::vector<Vertex_PCU> ringVerts;
			Vec2 position = selected->GetPosition();
			AddVertsForRing2D(ringVerts, position, selected->GetCollisionRadius(), m_entitySelectWidth, Rgba::GREEN);

			for (int i = 0; i < (int)ringVerts.size(); i++)
			{
				ringVerts[i].m_position.z = -0.01f;
			}

			g_renderContext->BindModelMatrix(Matrix44::IDENTITY);
			g_renderContext->DrawVertexArray(ringVerts);
		}
	}

	//Draw a hovered entity
	Entity* hovered = FindEntity(inputClass->m_hoverHandle);
	if (hovered != nullptr)
	{
		std::vector<Vertex_PCU> ringVerts;
		Vec2 position = hovered->GetPosition();
		AddVertsForRing2D(ringVerts, position, hovered->GetCollisionRadius() - 0.1f, m_entitySelectWidth, Rgba::WHITE);

		for (int i = 0; i < (int)ringVerts.size(); i++)
		{
			ringVerts[i].m_position.z = -0.01f;
		}

		g_renderContext->BindModelMatrix(Matrix44::IDENTITY);
		g_renderContext->DrawVertexArray(ringVerts);
	}

	//Draw the entity sprite
	g_renderContext->BindShader(Game::s_gameReference->m_defaultLit);
	g_renderContext->BindTextureView(0U, nullptr);

	for (int index = 0; index < (int)m_entities.size(); index++)
	{
		if(m_entities[index] == nullptr)
			continue;

		switch (m_entities[index]->GetType())
		{
		case PEON:
		case WARRIOR:
		{
			RenderIsoSpriteForEntity(*m_entities[index]);
		}
		break;
		case TREE:
		{
			RenderResourceEntity(*m_entities[index]);
		}
		break;
		case TOWNCENTER:
		{
			RenderTownCenter(*m_entities[index]);
		}
		break;
		default:
			break;
		}

		
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::RenderIsoSpriteForEntity(const Entity& entity) const
{
	IsoSpriteDefenition* isoSprite = &entity.m_animationSet[entity.m_currentState]->GetIsoSpriteAtTime(entity.m_currentAnimTime);
	DrawBillBoardedIsoSprites(entity.GetPosition(), entity.GetDirectionFacing(), *isoSprite, *Game::s_gameReference->m_RTSCam, entity.GetType(), Rgba::WHITE, entity.m_currentState);

	if (!entity.IsAlive())
		return;

	DrawHealthBar(entity);
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::DrawHealthBar(const Entity& entity) const
{
	//Draw the health bar
	Vec3 corners[4];

	float width = m_healthBarWidth;
	float height = m_healthBarHeight;
	
	float zHeight;
	switch (entity.GetType())
	{
	case PEON:
	case WARRIOR:
		zHeight = -1.5f;
		break;
	case TREE:
		zHeight = -3.f;
		break;
	case TOWNCENTER:
		zHeight = -4.f;
		break;
	}

	Vec2 pivot = m_healthBarPivot;

	corners[0] = Vec3::ZERO + height * Vec3::UP;
	corners[1] = Vec3::ZERO + height * Vec3::UP + width * Vec3::RIGHT;
	corners[2] = Vec3::ZERO;
	corners[3] = Vec3::ZERO + width * Vec3::RIGHT;

	Vec2 localOffset = -1.f * (pivot * Vec2(width, height));
	Vec3 worldOffset = localOffset.x * Vec3::RIGHT + localOffset.y * Vec3::UP;

	// offset so pivot point is at position
	for (uint i = 0; i < 4; ++i) {
		corners[i] += worldOffset;
	}

	CPUMesh mesh;

	AABB2 box = AABB2(corners[2], corners[1]);

	CPUMeshAddQuad(&mesh, box, Rgba::BLACK);
	m_quad->CreateFromCPUMesh<Vertex_Lit>(&mesh, GPU_MEMORY_USAGE_STATIC);

	//Billboard here
	RTSCamera* camera = Game::s_gameReference->m_RTSCam;
	Matrix44 mat = camera->GetModelMatrix();
	Matrix44 objectModel = Matrix44::IDENTITY;
	objectModel.SetRotationFromMatrix(objectModel, mat);

	objectModel = Matrix44::SetTranslation3D(Vec3(entity.GetPosition()) + Vec3(0.f, 0.f, zHeight), objectModel);

	g_renderContext->BindShader(g_renderContext->CreateOrGetShaderFromFile("default_unlit.xml"));
	g_renderContext->BindModelMatrix(objectModel);
	g_renderContext->BindTextureView(0U, nullptr);
	g_renderContext->DrawMesh(m_quad);

	//Making the actual health bar
	float ratio = entity.GetHealth() / entity.GetMaxHealth();
	width = ratio * m_healthBarWidth;

	corners[0] = Vec3::ZERO + height * Vec3::UP;
	corners[1] = Vec3::ZERO + height * Vec3::UP + width * Vec3::RIGHT;
	corners[2] = Vec3::ZERO;
	corners[3] = Vec3::ZERO + width * Vec3::RIGHT;

	localOffset = -1.f * (pivot * Vec2(width, height));
	worldOffset = localOffset.x * Vec3::RIGHT + localOffset.y * Vec3::UP;

	// offset so pivot point is at position
	for (uint i = 0; i < 4; ++i) {
		corners[i] += worldOffset;
	}

	box = AABB2(corners[2], corners[1]);

	Rgba drawColor = Rgba::GREEN;
	if (ratio < 0.8f && ratio > 0.3f)
	{
		drawColor = Rgba::YELLOW;
	}
	else if (ratio < 0.3f)
	{
		drawColor = Rgba::RED;
	}

	CPUMeshAddQuad(&mesh, box, drawColor);
	m_quad->CreateFromCPUMesh<Vertex_Lit>(&mesh, GPU_MEMORY_USAGE_STATIC);

	//Billboard here
	mat = camera->GetModelMatrix();
	objectModel = Matrix44::IDENTITY;
	objectModel.SetRotationFromMatrix(objectModel, mat);

	objectModel = Matrix44::SetTranslation3D(Vec3(entity.GetPosition()) + Vec3(0.f, 0.f, zHeight), objectModel);

	g_renderContext->BindShader(g_renderContext->CreateOrGetShaderFromFile("default_unlit.xml"));
	g_renderContext->BindModelMatrix(objectModel);
	g_renderContext->BindTextureView(0U, nullptr);
	g_renderContext->DrawMesh(m_quad);
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::RenderResourceEntity(const Entity& entity) const
{
	float ratio = entity.GetHealth() / entity.GetMaxHealth();
	//Render the model at entity position
	GPUMesh* mesh = nullptr;

	if (ratio >= 0.8f)
	{
		mesh = g_renderContext->CreateOrGetMeshFromFile(entity.GetMeshIDForState(SOURCE));
	}
	else if (ratio < 0.8f && ratio >= 0.3f)
	{
		mesh = g_renderContext->CreateOrGetMeshFromFile(entity.GetMeshIDForState(FULL));
	}
	else if (ratio < 0.3f)
	{
		mesh = g_renderContext->CreateOrGetMeshFromFile(entity.GetMeshIDForState(WEAK));
	}

	Matrix44 objectModel = Matrix44::IDENTITY;
	//objectModel = objectModel.MakeUniformScale3D(0.00390625f);
	objectModel = Matrix44::SetTranslation3D(Vec3(entity.GetPosition()), objectModel);

	if (mesh == nullptr)
	{
		ERROR_AND_DIE("The mesh to be rendered was nullptr");
	}

	g_renderContext->BindMaterial(g_renderContext->CreateOrGetMaterialFromFile(m_treeMaterialFile));
	g_renderContext->BindModelMatrix(objectModel);
	g_renderContext->DrawMesh(mesh);

	DrawHealthBar(entity);
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::RenderTownCenter(const Entity& entity) const
{
	//Render the model at entity position
	Matrix44 objectModel = Matrix44::IDENTITY;
	objectModel = Matrix44::SetTranslation3D(Vec3(entity.GetPosition()), objectModel);

	g_renderContext->BindMaterial(m_townCenter->m_material);
	g_renderContext->BindModelMatrix(objectModel);
	g_renderContext->DrawMesh(m_townCenter->m_mesh);

	DrawHealthBar(entity);
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::RenderBuildingPreview() const
{
	Matrix44 objectModel = Matrix44::IDENTITY;

	GameInput* input = Game::s_gameReference->m_gameInput;
	Vec2 castLocation = input->m_terrainCastLocation;
	Vec2 correctedPos = input->GetCorrectedMapPosition(castLocation, m_tileDimensions, m_townCenterOcc);

	Vec3 terrainPos = Vec3(correctedPos);
	objectModel = Matrix44::SetTranslation3D(terrainPos, objectModel);

	if (IsRegionOccupied(correctedPos, m_townCenterOcc))
	{
		g_renderContext->BindShader(m_redShader);
	}
	else
	{
		g_renderContext->BindMaterial(m_townCenter->m_material);
	}

	g_renderContext->BindModelMatrix(objectModel);
	g_renderContext->BindTextureView(0U, nullptr);
	g_renderContext->DrawMesh(m_townCenter->m_mesh);
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::DrawBillBoardedIsoSprites(const Vec2& position, const Vec3& orientation, const IsoSpriteDefenition& isoDef, const RTSCamera& camera, EntityTypeT type, const Rgba& drawColor, eAnimationType animState) const
{
	Matrix44 viewMat = camera.GetViewMatrix();
	Vec3 entityForwardRelativeToCamera = viewMat.TransformVector3D(orientation);
	//Get the correct sprite for the direction
	SpriteDefenition *sprite = &isoDef.GetSpriteForLocalDirection(entityForwardRelativeToCamera);
	//Now draw the sprite
	DrawBillBoardedSprite(position, *sprite, camera, type, drawColor, animState);
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::DrawBillBoardedSprite(const Vec3& position, const SpriteDefenition& sprite, const RTSCamera& camera, EntityTypeT type, const Rgba& drawColor, eAnimationType animState) const
{
	// tl - tr
	// |     | 
	// bl - br
	Vec3 corners[4];
	Vec2 uvs[4];
	
	float width = m_entityWidth;
	float height = m_entityHeight;
	Vec2 pivot = sprite.GetPivot();

	// technically right
	//Vec3 right = camera.GetCameraRight();
	//Vec3 up = camera.GetCameraUp();

	corners[0] = Vec3::ZERO + height * Vec3::UP;
	corners[1] = Vec3::ZERO + height * Vec3::UP + width * Vec3::RIGHT;
	corners[2] = Vec3::ZERO;
	corners[3] = Vec3::ZERO + width * Vec3::RIGHT;

	Vec2 localOffset = -1.f * (pivot * Vec2(width, height));
	Vec3 worldOffset = localOffset.x * Vec3::RIGHT + localOffset.y * Vec3::UP;
	// vec3 worldOffset = (vec4( localOffset, 0, 0 ) * camera->GetCameraMatrix()).xyz();

	// offset so pivot point is at position
	for (uint i = 0; i < 4; ++i) {
		corners[i] += worldOffset;
	}

	sprite.GetUVs(uvs[0], uvs[3]);
	std::swap(uvs[0].y, uvs[3].y);

	CPUMesh mesh;

	AABB2 box = AABB2(corners[2], corners[1]);

	CPUMeshAddQuad(&mesh, box, drawColor, uvs[0], uvs[3]);
	m_quad->CreateFromCPUMesh<Vertex_Lit>(&mesh, GPU_MEMORY_USAGE_STATIC);

	//Billboard here
	Matrix44 mat = camera.GetModelMatrix();
	Matrix44 objectModel = Matrix44::IDENTITY;
	objectModel.SetRotationFromMatrix(objectModel, mat);

	objectModel = Matrix44::SetTranslation3D(position, objectModel);
	g_renderContext->BindShader(g_renderContext->CreateOrGetShaderFromFile("default_unlit.xml"));

	switch (type)
	{
	case PEON:
		switch (animState)
		{
		case ANIMATION_ATTACK:
		{
			g_renderContext->BindTextureView(0U, Game::s_gameReference->m_peonAttackTexture);
		}
		break;
		default:
		{
			g_renderContext->BindTextureView(0U, Game::s_gameReference->m_peonTexture);
		}
		break;
		}
		break;
	case WARRIOR:
		switch (animState)
		{
		case ANIMATION_ATTACK:
		{
			g_renderContext->BindTextureView(0U, Game::s_gameReference->m_warriorAttackTexture);
		}
		break;
		default:
		{
			g_renderContext->BindTextureView(0U, Game::s_gameReference->m_warriorTexture);
		}
		break;
		}
		break;
	default:
		break;
	}

	g_renderContext->BindModelMatrix(objectModel);
	g_renderContext->DrawMesh(m_quad);
	g_renderContext->BindTextureView(0U, nullptr);
}

//------------------------------------------------------------------------------------------------------------------------------
void Map::SetOccupancyForUnit(const Vec2& position, const IntVec2& occupancy, bool isOccupied)
{
	int posX = (int)position.x;
	int posY = (int)position.y;

	int lowLimitX = posX - occupancy.x / 2;
	int highLimitX = posX + occupancy.x / 2 + 1;

	int lowLimitY = posY - occupancy.y / 2;
	int hightLimitY = posY + occupancy.y / 2 + 1;

	for (int xIndex = lowLimitX; xIndex <= highLimitX; xIndex++)
	{
		for (int yIndex = lowLimitY; yIndex <= hightLimitY; yIndex++)
		{
			int tileID = xIndex + (yIndex * m_tileDimensions.y);

			if (tileID < m_tileDimensions.x * m_tileDimensions.y)
			{
				m_mapOccupancy[tileID] = isOccupied;
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
bool Map::IsRegionOccupied(const Vec2& position, const IntVec2& occupancy) const
{
	int posX = (int)position.x;
	int posY = (int)position.y;

	int lowLimitX = posX - occupancy.x / 2;
	int highLimitX = posX + occupancy.x / 2 + 1;

	int lowLimitY = posY - occupancy.y / 2;
	int hightLimitY = posY + occupancy.y / 2 + 1;

	for (int xIndex = lowLimitX; xIndex <= highLimitX; xIndex++)
	{
		for (int yIndex = lowLimitY; yIndex <= hightLimitY; yIndex++)
		{
			int tileID = xIndex + (yIndex * m_tileDimensions.y);

			if (tileID < m_tileDimensions.x * m_tileDimensions.y)
			{
				auto elem = m_mapOccupancy.find(tileID);
				if (elem->second == true)
				{
					return true;
				}
			}
		}
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------------------------
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
Entity* Map::CreateEntity(const Vec2& pos, EntityTypeT entityType, int team )
{
	uint slot = GetFreeEntityIndex();
	uint cyclicID = GetNextCyclicID();

	GameHandle handle = GameHandle(cyclicID, slot);
	Entity *entity = new Entity(handle, pos);
	entity->SetTeam(team);

	switch (entityType)
	{
	case PEON:
	{
		entity->MakeFromXML(m_peonXMLFile);
		entity->SetType(PEON);
	}
	break;
	case WARRIOR:
	{
		entity->MakeFromXML(m_warriorXMLFile);
		entity->SetType(WARRIOR);
	}
	break;
	case TREE:
	{
		entity->MakeFromXML(m_treeXMLFile);
		entity->SetType(TREE);
	}
	break;
	case TOWNCENTER:
	{
		//This is some sketch bro
		entity->MakeFromXML(m_townCenterXMLFile);
		entity->SetType(TOWNCENTER);
		entity->SetAsBuilding(true);
		entity->SetIsBuilt(false);
		entity->SetMaxHealth(entity->GetHealth());
		entity->SetHealth(0.f);
	}
	break;
	default:
		break;
	}
	
	// you may have to grow this vector...
	m_entities[slot] = entity;
	return entity;
}

//------------------------------------------------------------------------------------------------------------------------------
Entity* Map::FindEntity(const GameHandle& handle) const
{
	if (m_entities.size() == 0 || handle == GameHandle::INVALID)
	{
		return nullptr;
	}

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
void Map::ResolveEntityCollisions()
{
	for (int entityIndex = 0; entityIndex < (int)m_entities.size(); ++entityIndex)
	{
		if(m_entities[entityIndex] == nullptr || !m_entities[entityIndex]->IsAlive())
			continue;

		for (int otherEntityIndex = entityIndex + 1; otherEntityIndex < (int)m_entities.size(); ++otherEntityIndex)
		{
			if (m_entities[otherEntityIndex] == nullptr || !m_entities[otherEntityIndex]->IsAlive())
				continue;

			//Push them out of each other
			if (DoDiscsOverlap(m_entities[entityIndex]->GetEditablePosition(), m_entities[entityIndex]->GetCollisionRadius(), m_entities[otherEntityIndex]->GetEditablePosition(), m_entities[otherEntityIndex]->GetCollisionRadius()))
			{
				if (m_entities[otherEntityIndex]->GetTeam() == 0 || m_entities[otherEntityIndex]->IsBuildingType())
				{
					PushDiscOutOfDisc(m_entities[entityIndex]->GetEditablePosition(), m_entities[entityIndex]->GetCollisionRadius(),
						m_entities[otherEntityIndex]->GetEditablePosition(), m_entities[otherEntityIndex]->GetCollisionRadius());
				}
				else if (m_entities[entityIndex]->GetTeam() == 0 || m_entities[entityIndex]->IsBuildingType())
				{
					PushDiscOutOfDisc(m_entities[otherEntityIndex]->GetEditablePosition(), m_entities[otherEntityIndex]->GetCollisionRadius(),
						m_entities[entityIndex]->GetEditablePosition(), m_entities[entityIndex]->GetCollisionRadius());
				}
				else
				{
					PushDiscsApart(m_entities[entityIndex]->GetEditablePosition(), m_entities[entityIndex]->GetCollisionRadius(),
						m_entities[otherEntityIndex]->GetEditablePosition(), m_entities[otherEntityIndex]->GetCollisionRadius());
				}
			}
		}
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

		if (entity != nullptr)
		{
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
		if(entity == nullptr)
			continue;

		if (selectionFrustum.ContainsPoint(entity->GetPosition()))
		{
			if (entity->IsAlive() && entity->IsSelectable() && entity->GetHandle() != GameHandle::INVALID)
			{
				if (entity->GetTeam() == Game::s_gameReference->GetCurrentTeam())
				{
					entity->SetSelectable(true);
					entityHandles.push_back(entity->GetHandle());
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
int Map::GetNumEntities() const
{
	return (int)m_entities.size();
}

//------------------------------------------------------------------------------------------------------------------------------
int Map::GetTownCenterCost() const
{
	return m_townCenterCost;
}

//------------------------------------------------------------------------------------------------------------------------------
Entity* Map::GetClosestEntityOfType(EntityTypeT type, const Vec2& position)
{
	Entity* closestEntity = nullptr;
	float distanceSq = 10000;

	for (int entityIndex = 0; entityIndex < GetNumEntities(); entityIndex++)
	{
		if (m_entities[entityIndex] == nullptr)
			continue;

		if (m_entities[entityIndex]->GetType() == type && m_entities[entityIndex]->IsAlive())
		{
			float checkDistSq = GetDistanceSquared2D(position, m_entities[entityIndex]->GetPosition());

			if (checkDistSq < distanceSq)
			{
				closestEntity = m_entities[entityIndex];
				distanceSq = checkDistSq;
			}
		}
	}

	return closestEntity;
}

//------------------------------------------------------------------------------------------------------------------------------
int Map::GetPeonCost() const
{
	return m_peonCost;
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
