//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/Capsule3D.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Sphere.hpp"
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Renderer/Material.hpp"
//Game Systems
#include "Game/GameCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class BitmapFont;
class ColorTargetView;
class GameInput;
class GPUMesh;
class Image;
class Map;
class Model;
class RTSCamera;
class Shader;
class SpriteAnimDefenition;
class StopWatch;
class Texture;
class Texture2D;
class TextureView;
class UIButton;
class UIWidget;
class UIRadioGroup;

struct Camera;

//------------------------------------------------------------------------------------------------------------------------------
enum GameState
{
	STATE_INIT,
	STATE_MENU,
	STATE_LOAD,
	STATE_PLAY,
	STATE_EDIT,
	STATE_NULL
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

	static bool				GoToGame(EventArgs& args);
	static bool				GoToEdit(EventArgs& args);
	static bool				ReLoadMap(EventArgs& args);
	static bool				ResumeGame(EventArgs& args);
	static bool				ReturnToMenu(EventArgs& args);
	static bool				QuitGame(EventArgs& args);

	static Game*			s_gameReference;


	Vec2					GetClientToUIScreenPosition2D( IntVec2 mousePosInClient, IntVec2 ClientBounds );

	//Startup functions
	void								StartUp();
	
	void								SetupMouseData();
	void								SetupCameras();

	void								PerformInitActions();
	void								CreateMenuUIWidgets();
	void								CreateEditUIWidgets();
	void								CreateGameUIWidgets();
	void								CreatePauseUIWidgets();

	void								GetandSetShaders();
	void								LoadGameTextures();
	void								LoadGameMaterials();
	void								CreateInitialMeshes();
	void								CreateInitialLight();
	void								LoadInitMesh();
	
	void								BeginFrame();
	void								EndFrame();

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
	void								RenderMenuUI() const;
	void								RenderGameState() const;
	void								RenderGameUI() const;
	void								RenderEditState() const;
	void								RenderEditUI() const;
	void								RenderControlsToUI() const;
	void								RenderPauseScreen() const;
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
	bool								m_inputSetup = false;
	bool								m_showGameControls = false;
	float								m_cameraSpeed = 0.3f; 

public:
	SoundID								m_testAudioID = NULL;
	
	bool								m_beginMapLoad = false;
	bool								m_beginEditLoad = false;

	TextureView*						m_textureTest = nullptr;
	TextureView*						m_boxTexture = nullptr;
	TextureView*						m_sphereTexture = nullptr;
	TextureView*						m_backgroundTexture = nullptr;
	BitmapFont*							m_squirrelFont = nullptr;
	Image*								m_testImage = nullptr;

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
	std::string							m_tonemapPath = "tonemap.mat";
	std::string							m_backgroundPath = "pixelArt.jpg";
	std::string							m_objectPath = "building/towncenter.mesh";
	std::string							m_objectMatPath = "building/towncenter.mat";

	Camera*								m_mainCamera = nullptr;
	Camera*								m_devConsoleCamera = nullptr;
	Camera*								m_UICamera = nullptr;
	Camera*								m_pauseCamera = nullptr;
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

	Model*								m_initMesh = nullptr;
	Matrix44							m_townCenterTransform;

	GPUMesh*							m_capsule = nullptr;
	Matrix44							m_capsuleModel;
	Capsule3D							m_capsuleCPU = Capsule3D(Vec3(0.f, 1.f, 0.f), Vec3::ZERO, 0.5f);
	Sphere								m_sphereDebug = Sphere(Vec3(0.f, 0.f, 0.f), 0.5f);

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
	Material*							m_toneMap = nullptr;
	bool								m_useMaterial = true;

	float								m_emissiveFactor = 0.f;
	float								m_emissiveStep = 0.1f;

	//Game data
	GameState							m_gameState = STATE_INIT;
	GameState							m_lastState = STATE_NULL;

	//Map (For now will be 1 single map)
	Map*								m_map = nullptr;
	GameInput*							m_gameInput = nullptr;

	//UI References
	UIWidget*							m_menuParent = nullptr;
	UIRadioGroup*						m_menuRadGroup = nullptr;
	UIRadioGroup*						m_editRadGroup = nullptr;
	UIButton*							m_playButton = nullptr;
	UIButton*							m_editButton = nullptr;

	UIWidget*							m_editParent = nullptr;
	UIWidget*							m_gameParent = nullptr;

	StopWatch*							m_stopWatch = nullptr;
	int									m_lapCounter = 0;

	//Stuff for fullscreen FX
	Texture2D*							m_renderTexture = nullptr;
	ColorTargetView*					m_renderCTV = nullptr;

	bool								m_isPaused = false;
	UniformBuffer*						m_tonemapUBO = nullptr;

	float								m_pauseTimer = 0.f;

	UIWidget*							m_pauseParent = nullptr;
	UIRadioGroup*						m_pauseRadGroup = nullptr;
	UIButton*							m_resumeButton = nullptr;
	UIButton*							m_returnMenuButton = nullptr;
	UIButton*							m_quitButton = nullptr;
	bool								m_loadingMesh = true;
	bool								m_returnToMenu = false;
};