//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Entity.hpp"
//Engine Systems
#include "Engine/Math/Capsule3D.hpp"
#include "Engine/Math/Ray3D.hpp"

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
}

//------------------------------------------------------------------------------------------------------------------------------
Entity::~Entity()
{
}

//------------------------------------------------------------------------------------------------------------------------------
void Entity::Update(float deltaTime)
{
	
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
void Entity::MoveTo(Vec2 target)
{
	m_targetPosition = target;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec2 Entity::GetPosition() const
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
	Capsule3D capsule = Capsule3D((Vec3(m_position) + m_orientation * m_height), m_position, m_radius);
	return capsule;
}

//------------------------------------------------------------------------------------------------------------------------------
bool Entity::RaycastHit(float *out, const Ray3D& ray) const
{
	Capsule3D capsule = CreateEntityCapsule();
	uint hits = Raycast(out, ray, capsule);

	if (hits == 0)
	{
		return false;
	}
	else
	{
		return true;
	}

}
