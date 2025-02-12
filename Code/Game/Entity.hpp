//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/Capsule3D.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/XMLUtils/XMLUtils.hpp"

//Game Systems
#include "Game/GameHandle.hpp"
#include "Game/Animator.hpp"
#include "Game/RTSTask.hpp"
#include "Game/GameTypes.hpp"
#include "Game/PathSolver.hpp"

struct IntRange;
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
	explicit Entity(GameHandle handle, Vec2 position, const std::string& xmlName);
	~Entity();

	void					MakeFromXML(const std::string& fileName);
	void					SetMeshIDsForResource(XMLElement* xmlElement);
	void					MakeAnimationsForEntity(XMLElement* xmlElement, const IntVec2& dimensions, const std::string& id);

	void					Update(float deltaTime);
	void					CheckEntityDeath();
	void					UpdateAnimationTime(float deltaTime);
	void					CheckIfTrainingUnit(float deltaTime);
	void					CheckIfEntityIsPathing();
	bool					HasEntityReachedPathTarget();
	void					ResumeGathering();
	void					UpdateAnimations(float deltaTime);
	void					CheckTasks();

	void					PerformMovement(const Vec2& displacement, float deltaTime);
	void					PerformAttack();
	void					PerformDropOff();
	void					PerformBuildActions();
	void					MakeWalkCycle(const SpriteSheet& spriteSheet, int numFrames, int spritesEachFrame, const std::string& entityName, float animTime);
	void					MakeIdleCycle(const SpriteSheet& spriteSheet, int numFrames, int spritesEachFrame, int idleColumn, const std::string& entityName, float animTime);
	void					MakeAttackCycle(const SpriteSheet& spriteSheet, int numFrames, int spritesEachFrame, const std::string& entityName, float animTime);
	void					MakeDeathCycle(const SpriteSheet& spriteSheet, int numFrames, int sprtesEachFrame, const std::string& entityName, float animTime, const IntRange& deathColumns);

	IsoSpriteDefenition		MakeIsoSpriteDef(const SpriteDefenition spriteDefenitions[], uint numDefenitions);

	IsoAnimDefenition		MakeIsoAnimDef(const SpriteSheet& spriteSheet, int startDefIndex, int endDefIndex, 
											float durationSeconds, const std::string& animName,
											const std::vector<IsoSpriteDefenition>& isoSpriteDefs,
											SpriteAnimPlaybackType playbackType = SPRITE_ANIM_PLAYBACK_ONCE);

	void					ResetTaskData();
	void					DamageUnit(Entity* target);
	void					GatherUnit(Entity* target);
	void					Destroy();
	void					SetDeadState();
	void					SetSelectable(bool isSelectable);

	bool					IsDestroyed() const;
	bool					IsSelectable() const;
	bool					IsGarbage() const;
	bool					IsAlive() const;
	bool					IsResource() const;
	bool					IsBuildingType() const;
	bool					IsBuilt() const;
	bool					IsStatic();

	void					SetPosition(Vec2 pos);
	void					ResetTargetPosition();
	void					MoveTo(Vec2 target);
	void					PathTo(Vec2 target);
	Vec2					GetPosition() const;
	float					GetCollisionRadius() const;
	Vec2&					GetEditablePosition();
	inline const Vec3&		GetOrientation() const { return m_orientation; }
	inline const Vec3&		GetDirectionFacing() const { return m_directionFacing; }
	GameHandle				GetHandle() const;

	Capsule3D				CreateEntityCapsule() const;

	//Entity Actions
	void					Follow(Entity* unitToFollow);
	void					Attack(Entity* unitToAttack);
	void					Gather(Entity* unitToGather);
	void					StopFollow();
	void					StopAttack();

	//Resource Actions
	void					SetAsResource(bool resource);
	const std::string&		GetMeshIDForState(ResourceMeshT meshType) const;

	//Building Actions
	void					SetUnitToBuild(Entity* unitToBuild);
	void					SetAsBuilding(bool building);
	void					SetIsBuilt(bool isbuilt);
	void					Build(const Vec2& buildLocation, EntityTypeT entityType);
	void					ConstructBuilding(float deltaTime);
	void					SetIsTrainingUnit(bool isTraining);
	bool					IsTrainingUnit() const;
	float					GetTrainingProgress() const;
	float					GetTrainingDuration() const;

	inline EntityTypeT		GetType() const { return m_type; }
	inline void				SetType(EntityTypeT type) { m_type = type; }

	inline int				GetTeam() const { return m_team; }
	inline void				SetTeam(int team) { m_team = team; }

	const float				GetHealth() const  { return m_health; }
	const int				GetCurrentResource() const { return m_currentResourceInventory; }
	const float				GetMaxHealth() const  { return m_maxHealth; }
	void					SetMaxHealth(float maxHealth) { m_maxHealth = maxHealth; }
	void					SetHealth(float health) { m_health = health; }
	void					TakeDamage(float damage) { m_health -= damage; }
	void					DrainResource(float damage);
	float					GetAttackDamage() { return m_attackDamage; }
	float					GetProximitySquared() { return m_proximitySquared; }

	bool					RaycastHit(float *out, const Ray3D& ray) const;

	//Task Handling
	void					EnqueueTask(RTSTask* task);
	void					ProcessTasks(); // process and free up memory 
	void					ClearTasks();   // just free up memory 

public:
	//Animation Data
	IsoAnimDefenition*	m_animationSet[eAnimationType::ANIMATION_COUNT];
	eAnimationType		m_currentState = ANIMATION_IDLE;
	eAnimationType		m_prevState = ANIMATION_IDLE;
	float				m_currentAnimTime = 0.f;

private:

	EntityTypeT		m_type = PEON;
	TextureView*	m_walkTexture = nullptr;
	TextureView*	m_attackTexture = nullptr;

	float			m_animSetTime = 1.f;

	GameHandle		m_handle;
	eEntityFlags	m_flags;

	// info
	Vec2			m_position = Vec2::ZERO;
	Vec2			m_targetPosition = Vec2::ZERO;
	float			m_collisionRadius = 0.5f;

	// stats
	float			m_speed = 2.f;
	int				m_team = 0;
	float			m_health = 40.f;
	float			m_maxHealth = 40.f;
	float			m_attackDamage = 5.f;
	float			m_deathTime = 5.f;
	bool			m_isAlive = true;
	bool			m_doingDamage = false;
	bool			m_isGarbage = false;
	bool			m_isGathering = false;

	bool			m_isTrainingUnit = false;
	float			m_trainingDuration = 5.f;
	float			m_trainingProgress = 0.f;

	int				m_currentResourceInventory = 0;
	int				m_totalResourceInventory = 20;

	//Resource Information
	bool			m_isResource = false;
	std::map<ResourceMeshT, std::string>	m_meshIDMap;
	bool			m_dropOffResources = false;

	//Build Information
	bool			m_isBuildingType = false;
	bool			m_isBuilt = false;
	Vec2			m_buildLocation = Vec2::ZERO;
	IntVec2			m_occupancy = IntVec2::ZERO;
	int				m_currentSupply = 0;
	int				m_maxSupply = 40;
	float			m_buildTime = 0.f;
	float			m_buildTimeLimit = 5.f;
	EntityTypeT		m_buildingType;

	// collision
	float			m_height = 1.f;
	float			m_radius = 0.5f;
	float			m_proximitySquared = 2.3f;
	float			m_buildingProximity = 15.f;
	Vec3			m_orientation = Vec3::BACK; //Am I standing? Am I lying down?

	Vec3			m_directionFacing = Vec3::UP;

	std::vector<RTSTask*>			m_taskQueue;

	//Unit pointers for tasks
	Entity*			m_unitToFollow = nullptr;
	Entity*			m_unitToAttack = nullptr;
	Entity*			m_unitToGather = nullptr;
	Entity*			m_unitToBuild = nullptr;

	Entity*			m_returnGatherUnit = nullptr;
	Entity*			m_closestTownCenter = nullptr;

	//Pathing
	Path*			m_unitPath = nullptr;
	PathSolver		m_pathSolver;

	Vec2			m_pathTarget = Vec2::NEGATIVE_ONE;
};

