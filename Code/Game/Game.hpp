//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/Material.hpp"
//Game Systems
#include "Game/GameCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class BitmapFont;
class GPUMesh;
class Image;
class Map;
class RTSCamera;
class Shader;
class SpriteAnimDefenition;
class Texture;
class TextureView;

struct Camera;

//------------------------------------------------------------------------------------------------------------------------------
enum GameState
{
	STATE_INIT,
	STATE_MENU,
	STATE_LOAD,
	STATE_PLAY,
	STATE_EDIT,
};

//------------------------------------------------------------------------------------------------------------------------------
class Game
{
public:
	Game();
	~Game();
	
	//Static methods
	static bool				TestEvent(EventArgs& args);
	static bool				ToggleLight1(EventArgs& args);
	static bool				ToggleLight2(EventArgs& args);
	static bool				ToggleLight3(EventArgs& args);
	static bool				ToggleLight4(EventArgs& args);
	static bool				ToggleAllPointLights(EventArgs& args);

	static Vec2				GetClientToWorldPosition2D(IntVec2 mousePosInClient, IntVec2 ClientBounds);

	//Startup functions
	void								StartUp();
	
	void								SetupMouseData();
	void								SetupCameras();

	void								PerformInitActions();

	void								GetandSetShaders();
	void								LoadGameTextures();
	void								LoadGameMaterials();
	void								CreateInitialMeshes();
	void								CreateInitialLight();
	
	//Debug test objects
	void								SetStartupDebugRenderObjects();
	void								DebugEnabled();

	//Input handling
	void								HandleKeyPressed( unsigned char keyCode );
	void								HandleKeyReleased( unsigned char keyCode );
	void								HandleCharacter( unsigned char charCode );

	//Light functions
	void								EnablePointLight( uint slot, const Vec3& position, const Vec3& direction,
															const Rgba& color = Rgba::WHITE, float intensity = 1.f,
															const Vec3& diffuseAttenuation = Vec3(1.f, 0.f, 0.f),
															const Vec3& specularAttenuation = Vec3(1.f, 0.f, 0.f)) const;
	
	void								EnableDirectionalLight( const Vec3& position, const Vec3& direction,
															const Rgba& color = Rgba::WHITE, float intensity = 1.f,
															const Vec3& diffuseAttenuation = Vec3(1.f, 0.f, 0.f),
															const Vec3& specularAttenuation = Vec3(1.f, 0.f, 0.f)) const;

	//Game render functions
	void								Render() const;
	void								RenderInitState() const;
	void								RenderLoadState() const;
	void								RenderMainMenuState() const;
	void								RenderGameState() const;
	void								RenderEditState() const;
	void								RenderCursor() const;
	void								RenderControlsToUI() const;
	void								RenderUsingMaterial() const;
	void								RenderUsingLegacy() const;

	//Debug render functions
	void								DebugRenderToScreen() const;
	void								DebugRenderToCamera() const;
	void								PostRender();

	//Update methods
	void								Update( float deltaTime );
	void								UpdateMouseInputs(float deltaTime);
	void								UpdateLightPositions();
	void								UpdateCamera( float deltaTime );
	void								ClearGarbageEntities();
	void								CheckXboxInputs();
	void								CheckCollisions();

	//Mouse Input handling
	bool								HandleMouseLBDown();
	bool								HandleMouseLBUp();
	bool								HandleMouseRBDown();
	bool								HandleMouseRBUp();
	bool								HandleMouseScroll(float wheelDelta);


	//Shut down
	void								Shutdown();
	bool								IsAlive();

private:
	bool								m_isGameAlive = false;
	bool								m_consoleDebugOnce = false;
	bool								m_devConsoleSetup = false;
	bool								m_isDebugSetup = false;
	float								m_cameraSpeed = 0.3f; 

public:
	SoundID								m_testAudioID = NULL;
	
	TextureView*						m_textureTest = nullptr;
	TextureView*						m_boxTexture = nullptr;
	TextureView*						m_sphereTexture = nullptr;
	BitmapFont*							m_squirrelFont = nullptr;
	Image*								m_testImage = nullptr;
	float								m_animTime = 0.f;

	//D3D11 stuff
	Shader*								m_shader = nullptr;
	Shader*								m_normalShader = nullptr;
	Shader*								m_defaultLit = nullptr;
	std::string							m_defaultShaderPath = "default_unlit.00.hlsl";
	std::string							m_shaderLitPath = "default_lit_PCUN.hlsl";
	std::string							m_normalColorShader = "normal_shader.hlsl";
	std::string							m_testImagePath = "Test_StbiFlippedAndOpenGL.png";
	std::string							m_boxTexturePath = "woodcrate.jpg";
	std::string							m_sphereTexturePath = "2k_earth_daymap.jpg";
	std::string							m_xmlShaderPath = "default_unlit.xml";
	std::string							m_materialPath = "couch.mat";

	Camera*								m_mainCamera = nullptr;
	Camera*								m_devConsoleCamera = nullptr;
	Camera*								m_UICamera = nullptr;
	RTSCamera*							m_RTSCam = nullptr;
	Rgba*								m_clearScreenColor = nullptr;
	
	float								m_camFOVDegrees = 60.f; //Desired Camera Field of View
	eRotationOrder						m_rotationOrder = ROTATION_ORDER_DEFAULT;
	Vec3								m_camPosition = Vec3::ZERO;

	float								m_devConsoleScreenWidth = 0.f;
	float								m_devConsoleScreenHeight = 0.f;

	//FOR ASSIGNMENT 4:
	// Define the shapes, and how are they positionedin the world; 
	GPUMesh*							m_cube = nullptr; 
	Matrix44							m_cubeTransform; // cube's model matrix

	GPUMesh*							m_sphere = nullptr;
	Matrix44							m_sphereTransform;   // sphere's model matrix

	GPUMesh*							m_quad = nullptr;

	GPUMesh*							m_baseQuad = nullptr;
	Matrix44							m_baseQuadTransform;

	//Lighting Assignment
	int									m_lightSlot;
	float								m_ambientIntensity = 1.f;
	float								m_ambientStep = 0.1f;

	bool								m_enableDirectional = true;
	bool								m_normalMode = false;

	//Light positions
	Vec3								m_dynamicLight0Pos = Vec3::ZERO;
	Vec3								m_dynamicLight1Pos = Vec3::ZERO;
	Vec3								m_dynamicLight2Pos = Vec3::ZERO;
	Vec3								m_dynamicLight3Pos = Vec3::ZERO;
	
	//Light movement
	float								m_ySpeed = 2.f;

	//Material
	Material*							m_testMaterial = nullptr;
	bool								m_useMaterial = true;

	float								m_emissiveFactor = 0.f;
	float								m_emissiveStep = 0.1f;

	//Game data
	GameState							m_gameState = STATE_INIT;

	//Map (For now will be 1 single map)
	Map*								m_map;
};