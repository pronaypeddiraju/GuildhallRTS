//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Capsule3D.hpp"

//Game Systems
#include "Game/GameHandle.hpp"
#include "Game/Animator.hpp"

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
	void					ResetTargetPosition();
	void					MoveTo(Vec2 target);
	Vec2					GetPosition() const;
	float					GetCollisionRadius() const;
	Vec2&					GetEditablePosition();
	inline const Vec3&		GetOrientation() const { return m_orientation; }
	GameHandle				GetHandle() const;

	Capsule3D				CreateEntityCapsule() const;

	//To handle events
	//void					HandleEvents(char const* eventName);

	// return just one result if it hits - return the closest result
	// depending on its physics collision
	bool					RaycastHit(float *out, const Ray3D& ray) const;

private:

	//Animation Data
	IsoAnimDefenition *m_animationSet[eAnimationType::ANIMATION_COUNT];
	// AnimationDefinition *m_currentAnimation; 
	eAnimationType m_currentState;
	eAnimationType m_prevState;

	GameHandle		m_handle;
	eEntityFlags	m_flags;

	// info
	Vec2			m_position = Vec2::ZERO;
	Vec2			m_targetPosition = Vec2::ZERO;
	float			m_collisionRadius = 0.75f;

	// stats
	float			m_speed = 2.f;

	// UI collision
	float			m_height = 1.f;
	float			m_radius = 0.5f;
	Vec3			m_orientation = Vec3::BACK; //Am I standing? Am I lying down?

	//SpriteData
	IsoSpriteDefenition* m_isoSpriteDef = nullptr;
};

