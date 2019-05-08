//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"

//Game Systems
#include "Game/GameHandle.hpp"

struct Ray3D;

//------------------------------------------------------------------------------------------------------------------------------

enum eEntityFlagBit : uint
{
	ENTITY_DESTROYED_BIT = BIT_FLAG(0),    // object is marked for destruction
	ENTITY_SELECTABLE_BIT = BIT_FLAG(1),    // entity is selectable by the player
};

typedef uint eEntityFlags;

//------------------------------------------------------------------------------------------------------------------------------
class Entity
{
public:
	Entity();
	explicit Entity(GameHandle handle, Vec2 position);
	~Entity();

	void					Update(float deltaTime);

	void					Destroy();
	void					SetSelectable(bool isSelectable);

	bool					IsDestroyed() const;
	bool					IsSelectable() const;

	void					SetPosition(Vec2 pos);
	void					MoveTo(Vec2 target);
	Vec2					GetPosition() const;
	GameHandle				GetHandle() const;

	// return just one result if it hits - return the closest result
	// depending on its physics collision
	bool					RaycastHit(float *out, const Ray3D& ray) const;

public:
	GameHandle		m_handle;
	eEntityFlags	m_flags;

	// info
	Vec2			m_position;
	Vec2			m_targetPosition;

	// stats
	float			m_speed;

	// UI collision
	float			m_height = 1.f;
	float			m_radius = 0.5f;
	Vec3			m_orientation = Vec3::BACK; //Am I standing? Am I lying down?
};

