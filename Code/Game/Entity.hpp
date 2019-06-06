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

	
	void					MakeWalkCycle(const SpriteSheet& peonSheet, int numFrames, int spritesEachFrame, const std::string& entityName);

	IsoSpriteDefenition		MakeIsoSpriteDef(const SpriteDefenition spriteDefenitions[], uint numDefenitions);

	IsoAnimDefenition		MakeIsoAnimDef(const SpriteSheet& spriteSheet, int startDefIndex, int endDefIndex, 
											float durationSeconds, const std::string& animName,
											const std::vector<IsoSpriteDefenition>& isoSpriteDefs,
											SpriteAnimPlaybackType playbackType = SPRITE_ANIM_PLAYBACK_ONCE);

	void					SetAnimation(IsoAnimDefenition& animDef, eAnimationType animType);

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
	inline const Vec3&		GetDirectionFacing() const { return m_directionFacing; }
	GameHandle				GetHandle() const;

	Capsule3D				CreateEntityCapsule() const;

	//To handle events
	//void					HandleEvents(char const* eventName);

	// return just one result if it hits - return the closest result
	// depending on its physics collision
	bool					RaycastHit(float *out, const Ray3D& ray) const;

private:

	//Animation Data
	IsoAnimDefenition* m_animationSet[eAnimationType::ANIMATION_COUNT];
	// AnimationDefinition *m_currentAnimation; 
	eAnimationType m_currentState;
	eAnimationType m_prevState;

	float			m_animFrameTime = 0.5f;

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

	Vec3			m_directionFacing = Vec3::UP;
};

