//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Entity.hpp"
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/XMLUtils/XMLUtils.hpp"
#include "Engine/Math/Capsule3D.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Ray3D.hpp"
#include "Engine/Renderer/SpriteDefenition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/RenderContext.hpp"
//Game Systems
#include "Game/Game.hpp"
#include "Game/IsoAnimDefenition.hpp"
#include "Game/RTSTask.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Entity::Entity()
{
	//Set the handle here
}

//------------------------------------------------------------------------------------------------------------------------------
Entity::Entity(GameHandle handle, Vec2 position)
{
	m_handle = handle;
	m_position = position;
	m_targetPosition = m_position;

	m_flags = SetBit(m_flags, ENTITY_SELECTABLE_BIT);
}

//------------------------------------------------------------------------------------------------------------------------------
Entity::Entity(GameHandle handle, Vec2 position, const std::string& xmlName)
{
	m_handle = handle;
	m_position = position;
	m_targetPosition = m_position;

	m_flags = SetBit(m_flags, ENTITY_SELECTABLE_BIT);

	MakeFromXML(xmlName);
}

//------------------------------------------------------------------------------------------------------------------------------
Entity::~Entity()
{
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::MakeFromXML(const std::string& fileName)
{
	//Open the xml file and parse it
	tinyxml2::XMLDocument entityDoc;
	entityDoc.LoadFile(fileName.c_str());

	if (entityDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		/*
		printf("\n >> Error loading XML file from %s ", fileName);
		printf("\n >> Error ID : %i ", shaderDoc.ErrorID());
		printf("\n >> Error line number is : %i", shaderDoc.ErrorLineNum());
		printf("\n >> Error name : %s", shaderDoc.ErrorName());
		*/
		ERROR_AND_DIE(">> Error loading Entity XML file ");
		return;
	}
	else
	{
		//We read everything fine. Now just shove all that data into the required place
		XMLElement* rootElement = entityDoc.RootElement();

		std::string id = ParseXmlAttribute(*rootElement, "id", "peon");
		m_health = ParseXmlAttribute(*rootElement, "health", m_health);
		m_speed = ParseXmlAttribute(*rootElement, "speed", m_speed);
		bool selectable = ParseXmlAttribute(*rootElement, "selectable", true);
		bool isResource = ParseXmlAttribute(*rootElement, "resource", true);

		SetSelectable(selectable);
		SetAsResource(isResource);

		//Read animation texture data
		rootElement = rootElement->FirstChildElement();

		if (!isResource)
		{
			//Follow pattern for non resource

			std::string textureName = ParseXmlAttribute(*rootElement, "walkTexture", "");
			m_walkTexture = g_renderContext->CreateOrGetTextureViewFromFile(textureName);
			textureName = ParseXmlAttribute(*rootElement, "attackTexture", "");
			m_walkTexture = g_renderContext->CreateOrGetTextureViewFromFile(textureName);

			Vec2 pivot = ParseXmlAttribute(*rootElement, "pivot", Vec2::ZERO);
			IntVec2 dimensions = ParseXmlAttribute(*rootElement, "sheetDimensions", IntVec2::ZERO);

			//Load the specific animations
			XMLElement* childElement = rootElement->FirstChildElement();

			while (childElement != nullptr)
			{
				MakeAnimationsForEntity(childElement, dimensions, id);				
				childElement = childElement->NextSiblingElement();
			}
		}
		else
		{
			//Follows pattern for models
			if (rootElement != nullptr)
			{
				std::string name = rootElement->Name();
				if (name == "collision")
				{
					m_radius = ParseXmlAttribute(*rootElement, "radius", m_radius);		
				}

				XMLElement* childElement = rootElement->NextSiblingElement();

				if (childElement != nullptr)
				{
					SetMeshIDsForResource(childElement);
				}
			}
		}
	}

}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::SetMeshIDsForResource(XMLElement* xmlElement)
{
	m_meshIDMap[SOURCE] = ParseXmlAttribute(*xmlElement, "src", "");
	m_meshIDMap[BASE] = ParseXmlAttribute(*xmlElement, "base", "");
	m_meshIDMap[FULL] = ParseXmlAttribute(*xmlElement, "full", "");
	m_meshIDMap[WEAK] = ParseXmlAttribute(*xmlElement, "weak", "");
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::MakeAnimationsForEntity(XMLElement* xmlElement, const IntVec2& dimensions, const std::string& id)
{
	std::string animID = ParseXmlAttribute(*xmlElement, "id", "idle");
	int numFrames = ParseXmlAttribute(*xmlElement, "numFrames", 1);
	int spritesEachFrame = ParseXmlAttribute(*xmlElement, "spritesEachFrame", 8);
	float animTime = ParseXmlAttribute(*xmlElement, "animTime", 1.f);

	SpriteSheet walkSheet = SpriteSheet(m_walkTexture, dimensions);
	SpriteSheet attackSheet = SpriteSheet(m_attackTexture, dimensions);

	if (animID == "idle")
	{
		int idleColumn = ParseXmlAttribute(*xmlElement, "idleColumn", 5);
		MakeIdleCycle(walkSheet, numFrames, spritesEachFrame, idleColumn, id, animTime);
	}
	else if (animID == "walk")
	{
		MakeWalkCycle(walkSheet, numFrames, spritesEachFrame, id, animTime);
	}
	else if (animID == "death")
	{
		IntRange deathColumns = ParseXmlAttribute(*xmlElement, "deathColumn", IntRange(5, 7));
		MakeDeathCycle(walkSheet, numFrames, spritesEachFrame, id, animTime, deathColumns);
	}
	else if (animID == "attack")
	{
		MakeAttackCycle(attackSheet, numFrames, spritesEachFrame, id, animTime);
	}
	else
	{
		ASSERT_RECOVERABLE(true, "Animation type not defined in project");
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::Update(float deltaTime)
{
	if (m_health <= 0 && m_isAlive)
	{
		//Die
		Game* game = Game::s_gameReference;
		game->m_deathSoundPlayback = g_audio->Play3DSound(game->m_deathSoundID, m_position, game->m_SFXChannel);

		m_position = m_targetPosition;
		m_prevState = m_currentState;
		ResetTaskData();
		m_currentState = ANIMATION_DIE;
		SetDeadState();
		m_currentAnimTime = 0.f;
	}

	if (!m_isAlive && m_currentAnimTime < m_deathTime)
	{
		m_currentAnimTime += deltaTime;
	}
	else if(!m_isAlive && m_currentAnimTime > m_deathTime)
	{
		Destroy();
	}

	CheckTasks();

	UpdateAnimations(deltaTime);
	m_currentAnimTime += deltaTime;

	//Process any tasks in the queue
	ProcessTasks();
}

void Entity::UpdateAnimations(float deltaTime)
{
	//Lerp to the destination
	Vec2 disp = m_targetPosition - m_position;
	float magnitude = disp.GetLength();

	if (m_isAlive)
	{
		//I'm alive, determine what state I should be in
		if (m_unitToAttack != nullptr)
		{
			Vec2 attackUnitPos = m_unitToAttack->GetPosition();
			float distanceSq = GetDistanceSquared2D(attackUnitPos, m_position);
			if (distanceSq < m_proximitySquared)
			{
				m_position = m_targetPosition;
				m_prevState = m_currentState;
				if (m_currentState != ANIMATION_ATTACK)
				{
					m_currentAnimTime = 0.f;
				}
				m_currentState = ANIMATION_ATTACK;
			}
			else
			{
				m_position += disp.GetNormalized() * m_speed * deltaTime;
				m_prevState = m_currentState;
				m_currentState = ANIMATION_WALK;
			}
		}
		else if (magnitude < m_speed * deltaTime)
		{
			m_position = m_targetPosition;
			m_prevState = m_currentState;
			m_currentState = ANIMATION_IDLE;
			m_currentAnimTime = 0.f;
		}
		else
		{
			m_position += disp.GetNormalized() * m_speed * deltaTime;
			m_prevState = m_currentState;
			m_currentState = ANIMATION_WALK;
		}
	}
}

void Entity::CheckTasks()
{
	//Check if I need to follow a unit
	if (m_unitToFollow != nullptr)
	{
		MoveTo(m_unitToFollow->GetPosition());
	}
	
	//Check if I need to attack a unit
	if (m_unitToAttack != nullptr)
	{
		//Am I next to the unit?
		Vec2 attackUnitPos = m_unitToAttack->GetPosition();
		if (GetDistanceSquared2D(attackUnitPos, m_position) < m_proximitySquared)
		{
			MoveTo(m_position);
			DamageUnit(m_unitToAttack);
		}
		else
		{
			MoveTo(attackUnitPos);
		}
	}
	
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::DamageUnit(Entity* target)
{
	if (target->GetHealth() <= 0)
	{
		m_unitToAttack = nullptr;
	}

	m_directionFacing = target->GetPosition() - m_position;
	m_directionFacing.Normalize();

	int frameNum = m_animationSet[m_currentState]->GetIsoSpriteFrameAtTime(m_currentAnimTime);
	if (frameNum == 3 && !m_doingDamage)
	{
		Game* game = Game::s_gameReference;
		game->m_attackSoundPlayback = g_audio->Play3DSound(game->m_attackSoundID, m_position, game->m_SFXChannel);

		target->TakeDamage(m_attackDamage);
		m_doingDamage = true;
	}
	else if(frameNum != 3)
	{
		m_doingDamage = false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::MakeWalkCycle(const SpriteSheet& spriteSheet, int numFrames, int spritesEachFrame, const std::string& entityName, float animTime)
{
	std::vector<IsoSpriteDefenition> isoDefs;
	std::vector<SpriteDefenition> spriteDefs;

	for (int j = 0; j < numFrames; j++)
	{
		for (int i = 0; i < spritesEachFrame; i++)
		{
			spriteDefs.push_back(SpriteDefenition(spriteSheet.GetSpriteDef(i * spritesEachFrame + j), Vec2(0.5, 0.25)));
		}

		isoDefs.push_back(MakeIsoSpriteDef(&spriteDefs[0], spritesEachFrame));
		spriteDefs.clear();
	}

	//Make the walk animation
	std::string animName = entityName + ".walk";
	m_animationSet[ANIMATION_WALK] = new IsoAnimDefenition(spriteSheet, 0, (numFrames - 1), animTime, animName, isoDefs, SPRITE_ANIM_PLAYBACK_LOOP);
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::MakeIdleCycle(const SpriteSheet& spriteSheet, int numFrames, int spritesEachFrame, int idleColumn, const std::string& entityName, float animTime)
{
	std::vector<IsoSpriteDefenition> isoDefs;
	std::vector<SpriteDefenition> spriteDefs;

	for (int j = 0; j < numFrames; j++)
	{
		for (int i = 0; i < spritesEachFrame; i++)
		{
			int spriteID = i * spritesEachFrame + j + idleColumn;
			spriteDefs.push_back(SpriteDefenition(spriteSheet.GetSpriteDef(spriteID), Vec2(0.5, 0.25)));
		}

		isoDefs.push_back(MakeIsoSpriteDef(&spriteDefs[0], spritesEachFrame));
		spriteDefs.clear();
	}

	//Make the walk animation
	std::string animName = entityName + ".idle";
	m_animationSet[ANIMATION_IDLE] = new IsoAnimDefenition(spriteSheet, 0, (numFrames - 1), animTime, animName, isoDefs, SPRITE_ANIM_PLAYBACK_ONCE);
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::MakeAttackCycle(const SpriteSheet& spriteSheet, int numFrames, int spritesEachFrame, const std::string& entityName, float animTime)
{
	std::vector<IsoSpriteDefenition> isoDefs;
	std::vector<SpriteDefenition> spriteDefs;

	for (int j = 0; j < numFrames; j++)
	{
		for (int i = 0; i < spritesEachFrame; i++)
		{
			spriteDefs.push_back(SpriteDefenition(spriteSheet.GetSpriteDef(i * spritesEachFrame + j), Vec2(0.5, 0.25)));
		}

		isoDefs.push_back(MakeIsoSpriteDef(&spriteDefs[0], spritesEachFrame));
		spriteDefs.clear();
	}

	//Make the walk animation
	std::string animName = entityName + ".attack";
	m_animationSet[ANIMATION_ATTACK] = new IsoAnimDefenition(spriteSheet, 0, (numFrames - 1), animTime, animName, isoDefs, SPRITE_ANIM_PLAYBACK_LOOP);
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::MakeDeathCycle(const SpriteSheet& spriteSheet, int numFrames, int spritesEachFrame, const std::string& entityName, float animTime, const IntRange& deathColumns)
{
	std::vector<IsoSpriteDefenition> isoDefs;
	std::vector<SpriteDefenition> spriteDefs;

	for (int j = 0; j < numFrames; j++)
	{
		for (int i = 0; i < spritesEachFrame; i++)
		{
			int spriteID = i * spritesEachFrame + j + deathColumns.minInt;
			spriteDefs.push_back(SpriteDefenition(spriteSheet.GetSpriteDef(spriteID), Vec2(0.5, 0.25)));
		}

		isoDefs.push_back(MakeIsoSpriteDef(&spriteDefs[0], spritesEachFrame));
		spriteDefs.clear();
	}

	//Make the death animation
	std::string animName = entityName + ".death";
	m_animationSet[ANIMATION_DIE] = new IsoAnimDefenition(spriteSheet, 0, (numFrames - 1), animTime, animName, isoDefs, SPRITE_ANIM_PLAYBACK_ONCE);
}

//------------------------------------------------------------------------------------------------------------------------------
IsoSpriteDefenition Entity::MakeIsoSpriteDef(const SpriteDefenition spriteDefenitions[], uint numDefenitions)
{
	IsoSpriteDefenition isoSpriteDef(spriteDefenitions, numDefenitions);
	return isoSpriteDef;
}

//------------------------------------------------------------------------------------------------------------------------------
IsoAnimDefenition Entity::MakeIsoAnimDef(const SpriteSheet& spriteSheet, int startDefIndex, int endDefIndex, float durationSeconds, const std::string& animName, const std::vector<IsoSpriteDefenition>& isoSpriteDefs, SpriteAnimPlaybackType playbackType /*= SPRITE_ANIM_PLAYBACK_ONCE*/)
{
	IsoAnimDefenition animDef(spriteSheet, startDefIndex, endDefIndex, durationSeconds, animName, isoSpriteDefs, playbackType);
	return animDef;
}

// ------------------------------------------------------------------------------------------------------------------------------
// void Entity::SetAnimation(IsoAnimDefenition& animDef, eAnimationType animType)
// {
// 	m_animationSet[animType] = &animDef;
// }

//------------------------------------------------------------------------------------------------------------------------------
void Entity::ResetTaskData()
{
	m_unitToAttack = nullptr;
	m_unitToAttack = nullptr;
	m_targetPosition = m_position;
	m_currentState = ANIMATION_IDLE;
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::Destroy()
{
	SetBit(m_flags, ENTITY_DESTROYED_BIT);
	m_isGarbage = true;
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::SetDeadState()
{
	m_isAlive = false;
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::SetSelectable(bool isSelectable)
{
	SetBitTo(m_flags, ENTITY_SELECTABLE_BIT, isSelectable);
}

//------------------------------------------------------------------------------------------------------------------------------
bool Entity::IsDestroyed() const
{
	return IsBitSet(m_flags, ENTITY_DESTROYED_BIT);
}

//------------------------------------------------------------------------------------------------------------------------------
bool Entity::IsSelectable() const
{
	return IsBitSet(m_flags, ENTITY_SELECTABLE_BIT);
}

//------------------------------------------------------------------------------------------------------------------------------
bool Entity::IsGarbage() const
{
	return m_isGarbage;
}

//------------------------------------------------------------------------------------------------------------------------------
bool Entity::IsAlive() const
{
	return m_isAlive;
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::SetPosition(Vec2 pos)
{
	m_position = pos;
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::ResetTargetPosition()
{
	m_targetPosition = m_position;
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::MoveTo(Vec2 target)
{
	m_directionFacing = Vec3(target) - m_position;
	m_directionFacing.Normalize();
	m_targetPosition = target;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 Entity::GetPosition() const
{
	return m_position;
}

//------------------------------------------------------------------------------------------------------------------------------
float Entity::GetCollisionRadius() const
{
	return m_collisionRadius;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2& Entity::GetEditablePosition()
{
	return m_position;
}

//------------------------------------------------------------------------------------------------------------------------------
GameHandle Entity::GetHandle() const
{
	return m_handle;
}

//------------------------------------------------------------------------------------------------------------------------------
Capsule3D Entity::CreateEntityCapsule() const
{
	//Account for radius + height here
	Capsule3D capsule = Capsule3D((Vec3(m_position) + m_orientation * m_height), m_position, m_radius);
	return capsule;
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::Follow(Entity* unitToFollow)
{
	m_unitToFollow = unitToFollow;
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::Attack(Entity* unitToAttack)
{
	m_unitToAttack = unitToAttack;
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::StopFollow()
{
	m_unitToFollow = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::StopAttack()
{
	m_unitToAttack = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::SetAsResource(bool resource)
{
	m_isResource = resource;
	if (resource)
	{
		m_team = 0;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
const std::string& Entity::GetMeshIDForState(ResourceMeshT meshType) const
{
	std::map<ResourceMeshT, std::string>::const_iterator requestedMeshID = m_meshIDMap.find(meshType);
	if (requestedMeshID != m_meshIDMap.end())
	{
		//Model requested exists in the map
		return requestedMeshID->second;
	}
	
	ASSERT_RECOVERABLE(true, "Mesh type doesn't exist");
}

//------------------------------------------------------------------------------------------------------------------------------
bool Entity::RaycastHit(float *out, const Ray3D& ray) const
{
	Capsule3D capsule = Capsule3D((Vec3(m_position) + Vec3(0.f, 0.f, -1.f) * m_height), m_position, m_radius);
	uint hits = Raycast(out, ray, capsule);

	if (hits > 0) 
	{
		g_devConsole->PrintString(Rgba::WHITE, Stringf("Hits: %u", hits));
		for (uint i = 0; i < hits; ++i)
		{
			g_devConsole->PrintString(Rgba::YELLOW, Stringf("  Time: %.2f", out[i]));
		}
	}

	if (hits == 0)
	{
		return false;
	}
	else
	{
		return true;
	}

}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::EnqueueTask(RTSTask* task)
{
	for (int index = 0; index < m_taskQueue.size(); index++)
	{
		if (m_taskQueue[index] == nullptr)
		{
			m_taskQueue[index] = task;
			return;
		}
	}

	m_taskQueue.push_back(task);
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::ProcessTasks()
{
	std::vector<RTSTask*>::iterator itr;
	itr = m_taskQueue.begin();

	int numTasks = (int)m_taskQueue.size();

	for (int index = 0; index < numTasks; index++)
	{
		if (m_taskQueue[index] != nullptr)
		{
			m_taskQueue[index]->Execute();
			delete m_taskQueue[index];
			m_taskQueue[index] = nullptr;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::ClearTasks()
{
	m_unitToFollow = nullptr;
	m_taskQueue.clear();
}
