//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Entity.hpp"
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/Capsule3D.hpp"
#include "Engine/Math/Ray3D.hpp"
#include "Engine/Renderer/SpriteDefenition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
//Game Systems
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
Entity::~Entity()
{
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::Update(float deltaTime)
{
	CheckTasks();

	//Lerp to the destination
	Vec2 disp = m_targetPosition - m_position;
	float magnitude = disp.GetLength();

	if (magnitude < m_speed * deltaTime)
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
		//m_currentAnimTime = 0.f;
	}

	m_currentAnimTime += deltaTime;

	//Process any tasks in the queue
	ProcessTasks();
}

void Entity::CheckTasks()
{
	if (m_unitToFollow != nullptr)
	{
		MoveTo(m_unitToFollow->GetPosition());
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::MakeWalkCycle(const SpriteSheet& spriteSheet, int numFrames, int spritesEachFrame, const std::string& entityName)
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
	m_animationSet[ANIMATION_WALK] = new IsoAnimDefenition(spriteSheet, 0, (numFrames - 1), m_animSetTime, animName, isoDefs, SPRITE_ANIM_PLAYBACK_LOOP);
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::MakeIdleCycle(const SpriteSheet& spriteSheet, int numFrames, int spritesEachFrame, int idleColumn, const std::string& entityName)
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
	m_animationSet[ANIMATION_IDLE] = new IsoAnimDefenition(spriteSheet, 0, (numFrames - 1), m_animSetTime, animName, isoDefs, SPRITE_ANIM_PLAYBACK_ONCE);
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

//------------------------------------------------------------------------------------------------------------------------------
void Entity::SetAnimation(IsoAnimDefenition& animDef, eAnimationType animType)
{
	m_animationSet[animType] = &animDef;
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::Destroy()
{
	SetBit(m_flags, ENTITY_DESTROYED_BIT);
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
	m_directionFacing = Vec3(target) - m_directionFacing;
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
void Entity::StopFollow()
{
	m_unitToFollow = nullptr;
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
