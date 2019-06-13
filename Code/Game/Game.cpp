//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Game.hpp"
//Engine Systems
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystems.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/StopWatch.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Ray3D.hpp"
#include "Engine/Math/Vertex_Lit.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/IsoSpriteDefenition.hpp"
#include "Engine/Renderer/Model.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/TextureView.hpp"

//Game Systems
#include "Game/App.hpp"
#include "Game/GameInput.hpp"
#include "Game/Map.hpp"
#include "Game/RTSCamera.hpp"
#include "Game/RTSCommand.hpp"
#include "Game/UIWidget.hpp"
#include "Entity.hpp"

//------------------------------------------------------------------------------------------------------------------------------
float g_shakeAmount = 0.0f;

RandomNumberGenerator* g_randomNumGen;
extern RenderContext* g_renderContext;	// Declare these first
extern AudioSystem* g_audio;
bool g_debugMode = false;

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Game::TestEvent(EventArgs& args)
{
	UNUSED(args);
	g_devConsole->PrintString(Rgba::YELLOW, "This a test event called from Game.cpp");
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Game::ToggleLight1( EventArgs& args )
{
	UNUSED(args);
	if(g_renderContext->m_cpuLightBuffer.lights[1].color.a != 0.f)
	{
		g_devConsole->PrintString(Rgba::RED, "Disabling Light 1");
		g_renderContext->m_cpuLightBuffer.lights[1].color.a = 0.f;
	}
	else
	{
		g_devConsole->PrintString(Rgba::GREEN, "Enabling Light 1");
		g_renderContext->m_cpuLightBuffer.lights[1].color.a = 1.f;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Game::ToggleLight2( EventArgs& args )
{
	UNUSED(args);
	if(g_renderContext->m_cpuLightBuffer.lights[2].color.a != 0.f)
	{
		g_devConsole->PrintString(Rgba::RED, "Disabling Light 2");
		g_renderContext->m_cpuLightBuffer.lights[2].color.a = 0.f;
	}
	else
	{
		g_devConsole->PrintString(Rgba::GREEN, "Enabling Light 2");
		g_renderContext->m_cpuLightBuffer.lights[2].color.a = 1.f;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Game::ToggleLight3( EventArgs& args )
{
	UNUSED(args);
	if(g_renderContext->m_cpuLightBuffer.lights[3].color.a != 0.f)
	{
		g_devConsole->PrintString(Rgba::RED, "Disabling Light 3");
		g_renderContext->m_cpuLightBuffer.lights[3].color.a = 0.f;
	}
	else
	{
		g_devConsole->PrintString(Rgba::GREEN, "Enabling Light 3");
		g_renderContext->m_cpuLightBuffer.lights[3].color.a = 1.f;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Game::ToggleLight4( EventArgs& args )
{
	UNUSED(args);
	if(g_renderContext->m_cpuLightBuffer.lights[4].color.a != 0.f)
	{
		g_devConsole->PrintString(Rgba::RED, "Disabling Light 4");
		g_renderContext->m_cpuLightBuffer.lights[4].color.a = 0.f;
	}
	else
	{
		g_devConsole->PrintString(Rgba::GREEN, "Enabling Light 4");
		g_renderContext->m_cpuLightBuffer.lights[4].color.a = 1.f;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Game::ToggleAllPointLights( EventArgs& args )
{
	UNUSED(args);
	for(int i = 1; i < 5; i++)
	{
		if(g_renderContext->m_cpuLightBuffer.lights[i].color.a != 0.f)
		{
			g_renderContext->m_cpuLightBuffer.lights[i].color.a = 0.f;
		}
		else
		{
			g_renderContext->m_cpuLightBuffer.lights[i].color.a = 1.f;
		}
	}
	g_devConsole->PrintString(Rgba::GREEN, "Toggled All Point Lights");
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Game::GoToGame( EventArgs& args )
{
	UNUSED(args);
	s_gameReference->m_returnToMenu = false;
	s_gameReference->m_lastState = s_gameReference->m_gameState;
	s_gameReference->m_gameState = STATE_LOAD;
	s_gameReference->m_beginMapLoad = true;
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Game::GoToEdit( EventArgs& args )
{
	UNUSED(args);
	s_gameReference->m_returnToMenu = false;
	s_gameReference->m_lastState = s_gameReference->m_gameState;
	s_gameReference->m_gameState = STATE_LOAD;
	s_gameReference->m_beginEditLoad = true;
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Game::ReLoadMap( EventArgs& args )
{
	IntVec2 mapDimensions = IntVec2::ZERO;

	//Print the data we read
	mapDimensions = args.GetValue("Map", IntVec2::ZERO);

	if(mapDimensions == IntVec2::ZERO)
	{
		g_devConsole->PrintString(DevConsole::CONSOLE_ERROR, "Cannot create map of size 0,0");
		return false;
	}

	//If we have reached this point, we have valid data
	if(s_gameReference != nullptr)
	{
		//The game is valid
		s_gameReference->m_map->Create(mapDimensions.x, mapDimensions.y);
	}

	return true;	
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Game::ResumeGame(EventArgs& args)
{
	UNUSED(args);
	s_gameReference->m_isPaused = false;
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Game::ReturnToMenu(EventArgs& args)
{
	UNUSED(args);
	s_gameReference->m_lastState = s_gameReference->m_gameState;
	s_gameReference->m_beginEditLoad = false;
	s_gameReference->m_beginMapLoad = false;
	s_gameReference->m_gameState = STATE_MENU;
	s_gameReference->m_returnToMenu = true;
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Game::QuitGame(EventArgs& args)
{
	UNUSED(args);
	g_theApp->HandleQuitRequested();
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC Game* Game::s_gameReference = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
Vec2 Game::GetClientToUIScreenPosition2D( IntVec2 mousePosInClient, IntVec2 ClientBounds )
{
	Clamp(static_cast<float>(mousePosInClient.x), 0.f, static_cast<float>(ClientBounds.x));
	Clamp(static_cast<float>(mousePosInClient.y), 0.f, static_cast<float>(ClientBounds.y));

	float posOnX = RangeMapFloat(static_cast<float>(mousePosInClient.x), 0.0f, static_cast<float>(ClientBounds.x), 0.f, m_gameInput->GetScreenWidth());
	float posOnY = RangeMapFloat(static_cast<float>(mousePosInClient.y), static_cast<float>(ClientBounds.y), 0.f, 0.f, m_gameInput->GetScreenHeight());

	return Vec2(posOnX, posOnY);
}

//------------------------------------------------------------------------------------------------------------------------------
Game::Game()
{
	m_isGameAlive = true;

	m_squirrelFont = g_renderContext->CreateOrGetBitmapFontFromFile("SquirrelFixedFont");
	g_devConsole->SetBitmapFont(*m_squirrelFont);
	g_debugRenderer->SetDebugFont(m_squirrelFont);

	m_shader = g_renderContext->CreateOrGetShaderFromFile(m_xmlShaderPath);
	m_shader->SetDepth(eCompareOp::COMPARE_LEQUAL, true);

	m_gameInput = new GameInput(this);

	s_gameReference = this;

	m_stopWatch = new StopWatch(nullptr);
	m_stopWatch->Start(3.f);
}

//------------------------------------------------------------------------------------------------------------------------------
Game::~Game()
{	m_isGameAlive = false;
	Shutdown();

	s_gameReference = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::StartUp()
{
	m_lastState = STATE_NULL;
	m_gameState = STATE_INIT;
	
	SetupMouseData();
	SetupCameras();

	g_devConsole->PrintString(Rgba::BLUE, "this is a test string");
	g_devConsole->PrintString(Rgba::RED, "this is also a test string");
	g_devConsole->PrintString(Rgba::GREEN, "damn this dev console lit!");
	g_devConsole->PrintString(Rgba::WHITE, "Last thing I printed");

	g_eventSystem->SubscribeEventCallBackFn("TestEvent", TestEvent);

	g_eventSystem->SubscribeEventCallBackFn("ToggleLight1", ToggleLight1);
	g_eventSystem->SubscribeEventCallBackFn("ToggleLight2", ToggleLight2);
	g_eventSystem->SubscribeEventCallBackFn("ToggleLight3", ToggleLight3);
	g_eventSystem->SubscribeEventCallBackFn("ToggleLight4", ToggleLight4);
	g_eventSystem->SubscribeEventCallBackFn("ToggleAllPointLights", ToggleAllPointLights);

	g_eventSystem->SubscribeEventCallBackFn( "GoToGame", GoToGame);
	g_eventSystem->SubscribeEventCallBackFn( "GoToEdit", GoToEdit);
	g_eventSystem->SubscribeEventCallBackFn("RemakeMap", ReLoadMap);
	
	g_eventSystem->SubscribeEventCallBackFn("ResumeGame", ResumeGame);
	g_eventSystem->SubscribeEventCallBackFn("ReturnToMenu", ReturnToMenu);
	g_eventSystem->SubscribeEventCallBackFn("QuitGame", QuitGame);
	   
	/*
	//Only to keep track of what input does what
	DebugRenderOptionsT options;
	options.space = DEBUG_RENDER_SCREEN;
	g_debugRenderer->DebugAddToLog(options, "F1 and F2 to increase/decrease ambient light intensity", Rgba::WHITE, 20000.f);
	g_debugRenderer->DebugAddToLog(options, "F3 to toggle directional light", Rgba::WHITE, 20000.f);
	g_debugRenderer->DebugAddToLog(options, "F4 to toggle normal or lit shaders", Rgba::WHITE, 20000.f);	
	*/

}

//------------------------------------------------------------------------------------------------------------------------------
void Game::SetupMouseData()
{
	IntVec2 clientCenter = g_windowContext->GetClientCenter();
	g_windowContext->SetClientMousePosition(clientCenter);

	g_windowContext->SetMouseMode(MOUSE_MODE_ABSOLUTE);
	g_windowContext->ShowMouse();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::SetupCameras()
{
	IntVec2 client = g_windowContext->GetTrueClientBounds();
	float aspect = (float)client.x / (float)client.y;

	//Create the Camera and setOrthoView
	m_mainCamera = new Camera();
	m_mainCamera->SetColorTarget(nullptr);

	//Create the UI Camera
	m_UICamera = new Camera();
	m_UICamera->SetColorTarget(nullptr);

	//Create the Pause Camera
	m_pauseCamera = new Camera();
	m_pauseCamera->SetColorTarget(nullptr);

	//Create a devConsole Cam
	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetColorTarget(nullptr);

	//Create a proper screen camera
	m_properScreenCamera = new Camera();
	m_properScreenCamera->SetColorTarget(nullptr);

	//Create a RTSCamera
	m_RTSCam = new RTSCamera();
	m_RTSCam->SetColorTarget(nullptr);
	m_RTSCam->SetPerspectiveProjection( m_camFOVDegrees, 0.1f, 100.0f, aspect);
	//m_RTSCam->SetOrthoView(Vec2(-CANVAS_HEIGHT * 0.5f * aspect, -CANVAS_HEIGHT * 0.5f), Vec2(CANVAS_HEIGHT * 0.5f * aspect, CANVAS_HEIGHT * 0.5f));

	//Set Projection Perspective for new Cam
	m_camPosition = Vec3(0.f, 0.f, -10.f);
	m_mainCamera->SetColorTarget(nullptr);
	m_mainCamera->SetPerspectiveProjection( m_camFOVDegrees, 0.1f, 100.0f, aspect);

	//Set the ortho perspective for the UI camera
	m_UICamera->SetOrthoView(Vec2(-CANVAS_HEIGHT * 0.5f * aspect, -CANVAS_HEIGHT * 0.5f), Vec2(CANVAS_HEIGHT * 0.5f * aspect, CANVAS_HEIGHT * 0.5f));
	m_pauseCamera->SetOrthoView(Vec2(-CANVAS_HEIGHT * 0.5f * aspect, -CANVAS_HEIGHT * 0.5f), Vec2(CANVAS_HEIGHT * 0.5f * aspect, CANVAS_HEIGHT * 0.5f));

	m_properScreenCamera->SetOrthoView(Vec2::ZERO, Vec2(client));

	m_clearScreenColor = new Rgba(0.f, 0.f, 0.5f, 1.f);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::PerformInitActions()
{
	GetandSetShaders();
	LoadGameTextures();
	LoadGameMaterials();
	CreateInitialMeshes();
	CreateInitialLight();
	LoadInitMesh();

	CreateMenuUIWidgets();
	CreateGameUIWidgets();
	CreateEditUIWidgets();
	CreatePauseUIWidgets();

	m_lastState = STATE_INIT;
	m_gameState = STATE_MENU;
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::SetStartupDebugRenderObjects()
{
	ColorTargetView* ctv = g_renderContext->GetFrameColorTarget();
	//Setup debug render client data
	g_debugRenderer->SetClientDimensions( ctv->m_height, ctv->m_width );

	//Setup Debug Options
	DebugRenderOptionsT options;
	options.mode = DEBUG_RENDER_ALWAYS;
	options.beginColor = Rgba::BLUE;
	options.endColor = Rgba::RED;

	//------------------------------------------------------------------------------------------------------------------------------
	// 2D Objects
	//------------------------------------------------------------------------------------------------------------------------------

	//Make 2D Point on screen
	g_debugRenderer->DebugRenderPoint2D(options, Vec2(10.f, 10.f), 5.0f);
	//Make 2D Point at screen center
	options.beginColor = Rgba::BLUE;
	options.endColor = Rgba::BLACK;
	g_debugRenderer->DebugRenderPoint2D(options, Vec2(0.f, 0.f), 10.f);

	options.beginColor = Rgba::YELLOW;
	options.endColor = Rgba::RED;
	//Draw a line in 2D screen space
	g_debugRenderer->DebugRenderLine2D(options, Vec2(ctv->m_width * -0.5f, ctv->m_height * -0.5f), Vec2(-150.f, -150.f), 20.f);

	//Draw a quad in 2D screen space
	options.beginColor = Rgba::GREEN;
	options.endColor = Rgba::RED;
	g_debugRenderer->DebugRenderQuad2D(options, AABB2(Vec2(-150.f, -150.f), Vec2(-100.f, -100.f)), 20.f);

	//Textured Quad
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::RED;
	g_debugRenderer->DebugRenderQuad2D(options, AABB2(Vec2(-200.f, -200.f), Vec2(-150.f, -150.f)), 20.f, m_textureTest);

	//Disc2D
	options.beginColor = Rgba::DARK_GREY;
	options.endColor = Rgba::ORANGE;
	g_debugRenderer->DebugRenderDisc2D(options, Disc2D(Vec2(100.f, 100.f), 25.f), 10.f);

	//Ring2D
	options.beginColor = Rgba::ORANGE;
	options.endColor = Rgba::DARK_GREY;
	g_debugRenderer->DebugRenderRing2D(options, Disc2D(Vec2(100.f, 100.f), 25.f), 10.f, 5.f);

	//Ring2D
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderRing2D(options, Disc2D(Vec2(150.f, 100.f), 2000.f), 10.f, 1.f);

	//Wired Quad
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderWireQuad2D(options, AABB2(Vec2(100.f, -100.f), Vec2(150.f, -50.f)), 20.f);

	//Text
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::RED;
	const char* text2D = "Read me bruh";
	g_debugRenderer->DebugRenderText2D(options, Vec2(-100.f, 200.f), Vec2(100.f, 200.f), text2D, DEFAULT_TEXT_HEIGHT, 20.f);

	//Arrow 2D
	options.beginColor = Rgba::GREEN;
	options.endColor = Rgba::GREEN;
	g_debugRenderer->DebugRenderArrow2D(options, Vec2(0.f, 0.f), Vec2(200.f, 200.f), 20.f, 5.f);

	//Arrow 2D
	options.beginColor = Rgba::BLUE;
	options.endColor = Rgba::BLUE;
	g_debugRenderer->DebugRenderArrow2D(options, Vec2(0.f, 0.f), Vec2(200.f, -200.f), 20.f, 5.f);

	//------------------------------------------------------------------------------------------------------------------------------
	// 3D Objects
	//------------------------------------------------------------------------------------------------------------------------------
	
	DebugRenderOptionsT options3D;
	options3D.space = DEBUG_RENDER_WORLD;
	options3D.beginColor = Rgba::GREEN;
	options3D.endColor = Rgba::RED;

	options3D.mode = DEBUG_RENDER_XRAY;
	//make a 3D point
	g_debugRenderer->DebugRenderPoint(options3D, Vec3(0.0f, 0.0f, 0.0f), 10000.0f );

	options3D.mode = DEBUG_RENDER_USE_DEPTH;
	//Make a 3D textured point
	options3D.beginColor = Rgba::BLUE;
	options3D.endColor = Rgba::RED;
	g_debugRenderer->DebugRenderPoint(options3D, Vec3(-10.0f, 0.0f, 0.0f), 20.f, 1.f, m_textureTest);

	options3D.mode = DEBUG_RENDER_XRAY;
	//Make a line in 3D
	options3D.beginColor = Rgba::BLUE;
	options3D.endColor = Rgba::BLACK;
	g_debugRenderer->DebugRenderLine(options3D, Vec3(0.f, 0.f, 5.f), Vec3(10.f, 0.f, 10.f), 2000.f);

	options3D.mode = DEBUG_RENDER_USE_DEPTH;
	//Make a line in 3D
	options3D.beginColor = Rgba::BLUE;
	options3D.endColor = Rgba::BLACK;
	g_debugRenderer->DebugRenderLine(options3D, Vec3(0.f, 0.f, 5.f), Vec3(10.f, 0.f, 10.f), 2000.f);

	//Make a sphere
	options3D.beginColor = Rgba::RED;
	options3D.endColor = Rgba::BLACK;
	g_debugRenderer->DebugRenderSphere(options3D, Vec3(0.f, 3.f, 0.f), 1.f, 10.f, nullptr);
	
	//Make a sphere
	options3D.beginColor = Rgba::GREEN;
	options3D.endColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderSphere(options3D, Vec3(0.f, -3.f, 0.f), 1.f, 200.f, m_sphereTexture);
	
	//Make a wire sphere
	options3D.beginColor = Rgba::WHITE;
	options3D.endColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderWireSphere(options3D, Vec3(0.f, -2.f, 0.f), 1.f, 200.f);
	
	//Make a cube
	options3D.beginColor = Rgba::DARK_GREY;
	options3D.endColor = Rgba::WHITE;
	AABB3 cube = AABB3::UNIT_CUBE;
	g_debugRenderer->DebugRenderBox(options3D, cube, Vec3(-5.f, -1.5f, 0.f), 20.f);

	//Make a wire cube
	options3D.beginColor = Rgba::DARK_GREY;
	options3D.endColor = Rgba::WHITE;
	g_debugRenderer->DebugRenderWireBox(options3D, cube, Vec3(-5.f, 1.5f, 0.f), 20.f);

	//Make a quad 3D no billboard
	options3D.beginColor = Rgba::WHITE;
	options3D.endColor = Rgba::RED;
	AABB2 quad = AABB2(Vec3(-1.f, -1.f, 0.f), Vec3(1.f, 1.f, 0.f));
	Vec3 position = Vec3(3.f, 2.f, 1.f);
	g_debugRenderer->DebugRenderQuad(options3D, quad, position, 2000.f, m_textureTest, false);

	//Make a quad 3D 
	options3D.beginColor = Rgba::WHITE;
	options3D.endColor = Rgba::RED;
	quad = AABB2(Vec3(-1.f, -1.f, 0.f), Vec3(1.f, 1.f, 0.f));
	position = Vec3(5.f, 2.f, 1.f);
	g_debugRenderer->DebugRenderQuad(options3D, quad, position, 2000.f, m_textureTest);

	//Make text
	options3D.beginColor = Rgba::WHITE;
	options3D.endColor = Rgba::RED;
	const char* text = "This is some text";
	g_debugRenderer->DebugRenderText3D(options3D, Vec3(1.f, 1.f, 1.f), Vec2(1.f, 1.f), text, 0.1f, 20000.f);

	//Make text non billboarded
	options3D.beginColor = Rgba::BLUE;
	options3D.endColor = Rgba::RED;
	const char* textNB = "Billboard this";
	g_debugRenderer->DebugRenderText3D(options3D, Vec3(1.f, 0.5f, 0.f), Vec2(-1.f, 1.f), textNB, 0.2f, 20000.f, false);

	//------------------------------------------------------------------------------------------------------------------------------
	//	LOG Objects
	//------------------------------------------------------------------------------------------------------------------------------
	//Setup Debug Options
	options.mode = DEBUG_RENDER_ALWAYS;
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::YELLOW;
	const char* debugText1 = "Debug Log Test";
	g_debugRenderer->DebugAddToLog(options, debugText1, Rgba::YELLOW, 10.f);

	//Setup Debug Options
	options.beginColor = Rgba::WHITE;
	options.endColor = Rgba::GREEN;
	const char* debugText2 = "This is another Debug String";
	g_debugRenderer->DebugAddToLog(options, debugText2, Rgba::GREEN, 20.f);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::HandleKeyPressed(unsigned char keyCode)
{
	if(g_devConsole->IsOpen())
	{
		g_devConsole->HandleKeyDown(keyCode);
		return;
	}

	m_gameInput->HandleKeyPressed(keyCode);

	switch( keyCode )
	{
		case UP_ARROW:
		{
			//Increase emissive factor
			m_emissiveFactor += m_emissiveStep;
		}
		break;
		case DOWN_ARROW:
		{
			//decrease emissive factor
			m_emissiveFactor -= m_emissiveStep;
		}
		break;
		case RIGHT_ARROW:
		case LEFT_ARROW:
		case SPACE_KEY:
		case N_KEY:
		break;
		case F1_KEY:
		{
			//Setup the ambient intensity to lesser
			m_ambientIntensity -= m_ambientStep;
		}
		break;
		case F2_KEY:
		{
			//Setup the ambient intensity to lesser
			m_ambientIntensity += m_ambientStep;
		}
		break;
		case F3_KEY:
		{
			//Toggle directional light
			m_enableDirectional = !m_enableDirectional;
		}
		break;
		case A_KEY:
		{
			//Handle left movement
			Vec3 worldMovementDirection = m_mainCamera->m_cameraModel.GetIVector() * -1.f;
			worldMovementDirection *= (m_cameraSpeed);

			m_camPosition += worldMovementDirection; 
		}
		break;
		case W_KEY:
		{
			//Handle forward movement
			Vec3 worldMovementDirection = m_mainCamera->m_cameraModel.GetKVector();
			worldMovementDirection *= (m_cameraSpeed); 

			m_camPosition += worldMovementDirection; 
		}
		break;
		case S_KEY:
		{
			//Handle backward movement
			Vec3 worldMovementDirection = m_mainCamera->m_cameraModel.GetKVector() * -1.f;
			worldMovementDirection *= (m_cameraSpeed); 

			m_camPosition += worldMovementDirection; 
		}
		break;
		case D_KEY:
		{
			//Handle right movement
			Vec3 worldMovementDirection = m_mainCamera->m_cameraModel.GetIVector();
			worldMovementDirection *= (m_cameraSpeed); 

			m_camPosition += worldMovementDirection; 
		}
		break;
		case F4_KEY:
		{
			//Set volume back to 1
			//g_audio->SetSoundPlaybackVolume(m_testPlayback, 1.0f);
			
			//Toggle Shader here
			m_normalMode = !m_normalMode;

			break;
		}
		case F5_KEY:
		{
			//Set volume to 0
			//g_audio->SetSoundPlaybackVolume(m_testPlayback, 0.0f);
			
			//toggle material or not
			m_useMaterial = !m_useMaterial;

			break;
		}
		case F6_KEY:
		{
			//Fire event
			g_eventSystem->FireEvent("TestEvent");
			break;
		}		
		case F7_KEY:
		{
			//Quit Debug
			g_eventSystem->FireEvent("Quit");
			break;
		}
		case NUM_1:
		{
			if (m_gameState != STATE_MENU)
			{
				if (!m_isPaused)
				{
					m_pauseTimer = 0.f;
				}
				m_isPaused = !m_isPaused;
			}
			break;
		}
		case NUM_6:
		{
			m_showGameControls = !m_showGameControls;
			break;
		}
		default:
		break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::DebugEnabled()
{
	g_debugMode = !g_debugMode;
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	delete m_menuParent;
	m_menuParent = nullptr;

	delete m_pauseParent;
	m_pauseParent = nullptr;

	delete m_map;
	m_map = nullptr;

	delete m_mainCamera;
	m_mainCamera = nullptr;

	delete m_pauseCamera;
	m_pauseCamera = nullptr;

	delete m_devConsoleCamera;
	m_devConsoleCamera = nullptr;

	delete m_UICamera;
	m_UICamera = nullptr;

	delete m_RTSCam;
	m_RTSCam = nullptr;

	delete m_properScreenCamera;
	m_properScreenCamera = nullptr;

	delete m_cube;
	m_cube = nullptr;

	delete m_sphere;
	m_sphere = nullptr;

	delete m_quad;
	m_quad = nullptr;

	delete m_baseQuad;
	m_baseQuad = nullptr;

	delete m_capsule;
	m_capsule = nullptr;

	delete m_initMesh;
	m_initMesh = nullptr;

	//FreeResources();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::HandleKeyReleased(unsigned char keyCode)
{
	if(g_devConsole->IsOpen())
	{
		g_devConsole->HandleKeyUp(keyCode);
		return;
	}

	m_gameInput->HandleKeyReleased(keyCode);

	//SoundID testSound = g_audio->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
	switch( keyCode )
	{
		case UP_ARROW:
		case RIGHT_ARROW:
		case LEFT_ARROW:
		case NUM_1:
// 		{
// 			m_isPaused = false;
// 			break;
// 		}
		//g_audio->PlaySound( m_testAudioID );
		break;
		default:
		break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::HandleCharacter( unsigned char charCode )
{
	if(g_devConsole->IsOpen())
	{
		g_devConsole->HandleCharacter(charCode);
		return;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::Render() const
{
	//Get the ColorTargetView from rendercontext
	ColorTargetView *colorTargetView = g_renderContext->GetFrameColorTarget();

	//Setup what we are rendering to
	m_mainCamera->SetColorTarget(colorTargetView);
	m_UICamera->SetColorTarget(colorTargetView);
	m_devConsoleCamera->SetColorTarget(colorTargetView);
	m_RTSCam->SetColorTarget(colorTargetView);
	m_pauseCamera->SetColorTarget(colorTargetView);
	m_properScreenCamera->SetColorTarget(colorTargetView);

	switch( m_gameState )
	{
	case STATE_INIT:
	{
		RenderInitState();
	}
	break;
	case STATE_MENU:
	{
		RenderMainMenuState();
	}
	break;
	case STATE_LOAD:
	{
		RenderLoadState();
	}
	break;
	case STATE_PLAY:
	{
		RenderGameState();

		if (m_gameInput->m_LMousePressed)
		{
			IntVec2 currentMousePos = g_windowContext->GetClientMousePosition();
			AABB2 selectionBox = AABB2(Vec2(m_gameInput->m_mousePosLBDown), Vec2(currentMousePos));

			std::vector<Vertex_PCU> verts;
			AddVertsForAABB2D(verts, selectionBox, Rgba(0.f, 1.f, 0.f, 0.3f));

			g_renderContext->BeginCamera(*m_properScreenCamera);
			g_renderContext->BindModelMatrix(Matrix44::IDENTITY);
			g_renderContext->BindShader(m_shader);
			g_renderContext->BindTextureView(0U, nullptr);
			//g_renderContext->DrawVertexArray(verts);
			g_renderContext->EndCamera();
		}
	}
	break;
	case STATE_EDIT:
	{
		RenderEditState();
	}
	break;
	default:
	break;
	}

	//Show the controls for the UI Camera
	//RenderControlsToUI();

	if(m_isPaused)
	{
		RenderPauseScreen();
	}

	if(g_devConsole->IsOpen())
	{	
		g_renderContext->BindShader(m_shader);
		g_renderContext->BindTextureViewWithSampler(0U, m_squirrelFont->GetTexture());
		g_renderContext->SetModelMatrix(Matrix44::IDENTITY);
		g_devConsole->Render(*g_renderContext, *m_devConsoleCamera, DEVCONSOLE_LINE_HEIGHT);
	}	

	//Uncomment this when debugging
	//DebugRenderToCamera();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderGameState() const
{
	// Move the camera to where it is in the scene
	Matrix44 camTransform = Matrix44::MakeFromEuler( m_mainCamera->GetEuler(), m_rotationOrder ); 
	camTransform = Matrix44::SetTranslation3D(m_camPosition, camTransform);
	m_mainCamera->SetModelMatrix(camTransform);

	g_renderContext->BeginCamera(*m_RTSCam); 

	g_renderContext->ClearColorTargets(Rgba::BLACK);

	float intensity = Clamp(m_ambientIntensity, 0.f, 1.f);
	g_renderContext->SetAmbientLight( Rgba::WHITE, intensity ); 

	float emissive = Clamp(m_emissiveFactor, 0.1f, 1.f);
	g_renderContext->m_cpuLightBuffer.emissiveFactor = emissive;

	// enable a point light as some position in the world with a normal quadratic falloff; 
	if(m_enableDirectional)
	{
		g_renderContext->DisableDirectionalLight();
	}
	else 
	{
		g_renderContext->EnableDirectionalLight();
	}

	g_renderContext->SetModelMatrix(Matrix44::IDENTITY);
	m_map->Render();

	g_renderContext->EndCamera();

	g_renderContext->BeginCamera(*m_UICamera); 
	g_renderContext->BindShader(m_shader);
	g_renderContext->BindTextureViewWithSampler(0U, nullptr);
	m_UICamera->SetModelMatrix(Matrix44::IDENTITY);

	g_renderContext->BindMaterial(m_testMaterial);

	RenderGameUI();

	g_renderContext->EndCamera();

}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderGameUI() const
{
	g_renderContext->BindShader(m_shader);
	g_renderContext->BindTextureViewWithSampler(0U, m_squirrelFont->GetTexture());
	std::vector<Vertex_PCU> textVerts;
	
	Vec2 textPos = Vec2(-640.0f, 340.f);
	int numSelected = (int)m_gameInput->m_selectionHandles.size();
	
	for (int i = 0; i < numSelected; i++)
	{
		Entity* entity = m_map->FindEntity(m_gameInput->m_selectionHandles[i]);
		if (entity != nullptr)
		{
			EntityTypeT type = entity->GetType();
			int team = entity->GetTeam();

			std::string text = "Entity Type: ";
			Rgba textColor;

			float currentHealth = entity->GetHealth();
			float maxHealth = entity->GetMaxHealth();

			float healthRatio = currentHealth / maxHealth;

			switch (type)
			{
			case PEON:
			{
				text += " PEON ";
			}
			break;
			case WARRIOR:
			{
				text += " WARRIOR ";
			}
			break;
			default:
				break;
			}
			text += " Team: ";
			text += std::to_string(team);

			if (healthRatio > 0.8f)
			{
				textColor = Rgba::GREEN;
			}
			else if (healthRatio > 0.3f)
			{
				textColor = Rgba::YELLOW;
			}
			else
			{
				textColor = Rgba::RED;
			}

			m_squirrelFont->AddVertsForText2D(textVerts, textPos - Vec2(0.f, 10.f * i), 10.f, text, textColor);
		}
	}

	g_renderContext->DrawVertexArray(textVerts);

}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderEditState() const
{
	if (m_lastState == STATE_LOAD)
	{
		RenderLoadState();
		return;
	}

	// Move the camera to where it is in the scene
	Matrix44 camTransform = Matrix44::MakeFromEuler( m_mainCamera->GetEuler(), m_rotationOrder ); 
	camTransform = Matrix44::SetTranslation3D(m_camPosition, camTransform);
	m_mainCamera->SetModelMatrix(camTransform);

	g_renderContext->BeginCamera(*m_RTSCam);

	g_renderContext->ClearColorTargets(Rgba::BLACK);

	float intensity = Clamp(m_ambientIntensity, 0.f, 1.f);
	g_renderContext->SetAmbientLight( Rgba::WHITE, intensity ); 

	float emissive = Clamp(m_emissiveFactor, 0.1f, 1.f);
	g_renderContext->m_cpuLightBuffer.emissiveFactor = emissive;

	// enable a point light as some position in the world with a normal quadratic falloff; 
	if(m_enableDirectional)
	{
		g_renderContext->DisableDirectionalLight();
	}
	else 
	{
		g_renderContext->EnableDirectionalLight();
	}

	g_renderContext->SetModelMatrix(Matrix44::IDENTITY);
	m_map->Render();

	g_renderContext->SetModelMatrix(m_townCenterTransform);
	g_renderContext->BindMaterial(m_initMesh->m_material);
	g_renderContext->DrawMesh(m_initMesh->m_mesh);

	g_renderContext->EndCamera();

	g_renderContext->BeginCamera(*m_UICamera); 
	g_renderContext->BindShader(m_shader);
	g_renderContext->BindTextureViewWithSampler(0U, nullptr);
	m_UICamera->SetModelMatrix(Matrix44::IDENTITY);

	std::vector<Vertex_PCU> textVerts;
	AABB2 infoBox = AABB2(Vec2(-640.0f, 340.f), Vec2(640.f, 360.f));
	m_squirrelFont->AddVertsForTextInBox2D(textVerts, infoBox, 20.f, "EDIT MODE", Rgba::YELLOW, 1.f, Vec2::ALIGN_CENTERED);
	g_renderContext->BindTextureViewWithSampler(0U, m_squirrelFont->GetTexture());
	g_renderContext->DrawVertexArray(textVerts);

	g_renderContext->EndCamera();

	RenderEditUI();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderEditUI() const
{
	IntVec2 client = g_windowContext->GetTrueClientBounds();
	Vec2 boundsSize = Vec2((float)client.x, (float)client.y); 

	m_editParent->UpdateBounds(AABB2(Vec2(0.f, 0.f), Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT, UI_SCREEN_HEIGHT)));

	g_renderContext->BeginCamera(*m_UICamera);	
	
	m_editParent->Render();

	g_renderContext->EndCamera();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderControlsToUI() const
{
	g_renderContext->BindShader(m_shader);
	g_renderContext->BindTextureViewWithSampler(0U, m_squirrelFont->GetTexture());
	g_renderContext->SetModelMatrix(Matrix44::IDENTITY);

	g_renderContext->BeginCamera(*m_UICamera); 
	//g_renderContext->ClearColorTargets(Rgba::BLACK);

	std::vector<Vertex_PCU> textVerts;
	AABB2 infoBox = AABB2(Vec2(-640.0f, 340.f), Vec2(640.f, 360.f));
	m_squirrelFont->AddVertsForTextInBox2D(textVerts, infoBox, 20.f, "Num Key 1 : STATE_INIT", Rgba::WHITE, 1.f, Vec2::ALIGN_LEFT_CENTERED);
	
	infoBox = AABB2(Vec2(-640.0f, 320.f), Vec2(640.f, 340.f));
	m_squirrelFont->AddVertsForTextInBox2D(textVerts, infoBox, 20.f, "Num Key 2 : STATE_MENU", Rgba::WHITE, 1.f, Vec2::ALIGN_LEFT_CENTERED);
	
	infoBox = AABB2(Vec2(-640.0f, 300.f), Vec2(640.f, 320.f));
	m_squirrelFont->AddVertsForTextInBox2D(textVerts, infoBox, 20.f, "Num Key 3 : STATE_LOAD", Rgba::WHITE, 1.f, Vec2::ALIGN_LEFT_CENTERED);
	
	infoBox = AABB2(Vec2(-640.0f, 280.f), Vec2(640.f, 300.f));
	m_squirrelFont->AddVertsForTextInBox2D(textVerts, infoBox, 20.f, "Num Key 4 : STATE_PLAY", Rgba::WHITE, 1.f, Vec2::ALIGN_LEFT_CENTERED);

	infoBox = AABB2(Vec2(-640.0f, 260.f), Vec2(640.f, 280.f));
	m_squirrelFont->AddVertsForTextInBox2D(textVerts, infoBox, 20.f, "Num Key 5 : STATE_EDIT", Rgba::WHITE, 1.f, Vec2::ALIGN_LEFT_CENTERED);

	g_renderContext->DrawVertexArray(textVerts);

	g_renderContext->EndCamera();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderPauseScreen() const
{
	if (m_gameState == STATE_MENU)
	{
		return;
	}

	if (g_devConsole->GetFrameCount() > 1)
	{
		//Decrement our stop watch here
		//float time = static_cast<float>(GetCurrentTimeSeconds());
		//float intensity = (sin(time) + 1.f);

		TonemapBufferT buffer;
		buffer.intensity = m_pauseTimer;
		
		m_toneMap->SetUniforms(&buffer, sizeof(buffer));
		
		g_renderContext->ApplyEffect(m_toneMap);
		g_renderContext->BindTextureView(0U, nullptr);
	}

	if (m_isPaused)
	{
		m_pauseCamera->SetColorTarget(g_renderContext->GetFrameColorTarget());
		m_pauseParent->UpdateBounds(AABB2(Vec2(0.f, 0.f), Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT, UI_SCREEN_HEIGHT)));

		g_renderContext->BeginCamera(*m_pauseCamera);

		m_pauseParent->Render();

		g_renderContext->EndCamera();
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderInitState() const
{
	g_renderContext->BindShader(m_shader);
	g_renderContext->BindTextureViewWithSampler(0U, m_squirrelFont->GetTexture());
	g_renderContext->SetModelMatrix(Matrix44::IDENTITY);
	
	g_renderContext->BeginCamera(*m_UICamera); 
	g_renderContext->ClearColorTargets(Rgba::BLACK);

	std::vector<Vertex_PCU> textVerts;
	AABB2 titleBox = AABB2(Vec2(-100.0f, -100.f), Vec2(100.f, 100.f));
	m_squirrelFont->AddVertsForTextInBox2D(textVerts, titleBox, 10.f, "Initializing Game.", Rgba::WHITE);
	g_renderContext->DrawVertexArray(textVerts);

	/*
	g_renderContext->BindTextureViewWithSampler(0U, nullptr);
	std::vector<Vertex_PCU> boxVerts;
	AddVertsForBoundingBox(boxVerts, titleBox, Rgba::RED, 10.f);
	g_renderContext->DrawVertexArray(boxVerts);
	*/

	g_renderContext->EndCamera();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderLoadState() const
{
	g_renderContext->BindShader(m_shader);
	g_renderContext->BindTextureViewWithSampler(0U, m_squirrelFont->GetTexture());
	g_renderContext->SetModelMatrix(Matrix44::IDENTITY);

	g_renderContext->BeginCamera(*m_UICamera); 
	g_renderContext->ClearColorTargets(Rgba::BLACK);

	std::vector<Vertex_PCU> textVerts;
	AABB2 titleBox = AABB2(Vec2(-100.0f, -100.f), Vec2(100.f, 100.f));
	m_squirrelFont->AddVertsForTextInBox2D(textVerts, titleBox, 10.f, "Loading....", Rgba(1.f, 1.f, 1.f, 0.5f));
	g_renderContext->DrawVertexArray(textVerts);

	g_renderContext->EndCamera();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderMainMenuState() const
{
	g_renderContext->BindShader(m_shader);

	g_renderContext->BeginCamera(*m_UICamera); 
	g_renderContext->ClearColorTargets(Rgba::BLACK);

	g_renderContext->BindTextureViewWithSampler(0U, m_backgroundTexture);
	std::vector<Vertex_PCU> boxVerts;
	AABB2 screenBox = AABB2(Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT * -0.5f, UI_SCREEN_HEIGHT * -0.5f), Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT * 0.5f, UI_SCREEN_HEIGHT * 0.5f));
	AddVertsForAABB2D(boxVerts, screenBox, Rgba::WHITE);
	g_renderContext->DrawVertexArray(boxVerts);

	g_renderContext->EndCamera();

	RenderMenuUI();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderMenuUI() const
{
	//Render the Menu UI 
	/*
	IntVec2 client = g_windowContext->GetTrueClientBounds();
	Vec2 boundsSize = Vec2(client.x, client.y); 
	m_menuParent->UpdateBounds(AABB2(Vec2(UI_SCREEN_HEIGHT * UI_SCREEN_ASPECT * -0.5f, UI_SCREEN_HEIGHT * -0.5f), Vec2(UI_SCREEN_HEIGHT * 0.5f, UI_SCREEN_HEIGHT * 0.5f)));
	*/

	m_menuParent->UpdateBounds(AABB2(Vec2(0.f, 0.f), Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT, UI_SCREEN_HEIGHT)));

	g_renderContext->BeginCamera(*m_UICamera);	

	m_menuParent->Render();

	g_renderContext->EndCamera();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderUsingMaterial() const
{
	g_renderContext->BindMaterial(m_testMaterial);

	//Render the cube
	//g_renderContext->BindTextureViewWithSampler(0U, m_boxTexturePath);  
	g_renderContext->SetModelMatrix(m_cubeTransform);
	g_renderContext->DrawMesh( m_cube ); 

	//Render the sphere
	//g_renderContext->BindTextureViewWithSampler(0U, m_sphereTexturePath); 
	g_renderContext->SetModelMatrix( m_sphereTransform ); 
	g_renderContext->DrawMesh( m_sphere ); 

	//Render the Quad
	//g_renderContext->BindTextureViewWithSampler(0U, nullptr);
	g_renderContext->SetModelMatrix(Matrix44::IDENTITY);
	g_renderContext->DrawMesh( m_quad );

	//Render the capsule here
	g_renderContext->SetModelMatrix(m_capsuleModel);
	g_renderContext->DrawMesh(m_capsule);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::RenderUsingLegacy() const
{
	//Bind the shader we are using (This case it's the default shader we made in Shaders folder)
	//g_renderContext->BindShader( m_shader );
	if(m_normalMode)
	{
		g_renderContext->BindShader( m_normalShader );
	}
	else
	{
		g_renderContext->BindShader( m_defaultLit );
	}

	//Render the cube
	g_renderContext->BindTextureViewWithSampler(0U, m_boxTexturePath);  
	g_renderContext->SetModelMatrix(m_cubeTransform);
	g_renderContext->DrawMesh( m_cube ); 

	//Render the sphere
	g_renderContext->BindTextureViewWithSampler(0U, m_sphereTexturePath); 
	g_renderContext->SetModelMatrix( m_sphereTransform ); 
	g_renderContext->DrawMesh( m_sphere ); 

	//Render the Quad
	g_renderContext->BindTextureViewWithSampler(0U, nullptr);
	g_renderContext->SetModelMatrix(Matrix44::IDENTITY);
	g_renderContext->DrawMesh( m_quad );

	//Render the capsule her
	g_renderContext->SetModelMatrix(m_capsuleModel);
	g_renderContext->DrawMesh(m_capsule);

}

//------------------------------------------------------------------------------------------------------------------------------
void Game::DebugRenderToScreen() const
{
	Camera& debugCamera = g_debugRenderer->Get2DCamera();
	debugCamera.m_colorTargetView = g_renderContext->GetFrameColorTarget();
	
	g_renderContext->BindShader(m_shader);
	g_renderContext->BeginCamera(debugCamera);
	
	g_debugRenderer->DebugRenderToScreen();

	g_renderContext->EndCamera();
	
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::DebugRenderToCamera() const
{
	Camera& debugCamera3D = *m_RTSCam;
	debugCamera3D.m_colorTargetView = g_renderContext->GetFrameColorTarget();

	g_renderContext->BindShader(m_shader);
	g_renderContext->BeginCamera(debugCamera3D);
	
	g_debugRenderer->Setup3DCamera(&debugCamera3D);
	g_debugRenderer->DebugRenderToCamera();

	g_renderContext->EndCamera();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::PostRender()
{
	//Debug bools
	m_consoleDebugOnce = true;

	ColorTargetView* ctv = g_renderContext->GetFrameColorTarget();

	if(!m_isDebugSetup)
	{
		//Setup debug render client data
		g_debugRenderer->SetClientDimensions( ctv->m_height, ctv->m_width );

		m_isDebugSetup = true;
	}

	if(m_beginMapLoad)
	{
		m_lastState = m_gameState;
		m_gameState = STATE_PLAY;
	}

	//Uncomment this when debugging
	//DebugRenderToScreen();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::Update( float deltaTime )
{
	//First just return if we are paused
	if (m_pauseTimer < m_stopWatch->GetDuration())
	{
		//Add any logic to be done for pause state here	
		m_pauseTimer += deltaTime;
	}

	if (m_isPaused)
	{
		return;
	}

	if (m_gameState == STATE_MENU)
	{
		m_isPaused = false;
	}

	//Update all game input
	CheckXboxInputs();
	m_gameInput->Update(deltaTime);

	Vec2 framePan = m_gameInput->GetFramePan();
	m_RTSCam->PanFocalPoint(framePan);

	float angleOffset = m_gameInput->GetFrameRotation();
	m_RTSCam->SetAngleOffset(angleOffset);

	m_RTSCam->Update(deltaTime);

	//Update the moving lights
	UpdateLightPositions();

	//Update the FX Buffer
	//g_renderContext->UpdateFxBuffer(m_stopWatch->GetNormalizedElapsedTime());

	ProcessCommands();

	if (m_gameState == STATE_PLAY)
	{
		m_map->Update(deltaTime);
	}

	//If we can load the map, let's load it
	if(m_beginMapLoad)
	{
		m_lastState = STATE_LOAD;

		if(m_map == nullptr)
		{
			m_map = new Map();
			m_map->Load("InitMap");
		}

		m_RTSCam->SetFocusBounds(m_map->GetXYBounds());
	}

	//If we can load the edit data let's do that too (Move this to a better place)
	if(m_beginEditLoad)
	{
		if(m_map == nullptr)
		{
			m_map = new Map();
			m_map->Load("InitMap");
		}

		LoadInitMesh();

		m_RTSCam->SetFocusBounds(m_map->GetXYBounds());

		m_lastState = m_gameState;
		m_gameState = STATE_EDIT;
	}

	if(g_devConsole->GetFrameCount() > 1 && !m_devConsoleSetup)
	{
		//We have rendered the 1st frame
		PerformInitActions();

		ColorTargetView* target = g_renderContext->GetFrameColorTarget();
		float aspect = (float)target->m_width / (float)target->m_height;

		m_devConsoleCamera->SetOrthoView(Vec2(-WORLD_WIDTH * 0.5f * aspect, -WORLD_HEIGHT * 0.5f), Vec2(WORLD_WIDTH * 0.5f * aspect, WORLD_HEIGHT * 0.5f));
		m_devConsoleSetup = true;
	}

	g_renderContext->m_frameCount++;

	//Update the mouse position
	IntVec2 intVecPos = g_windowContext->GetClientMousePosition();
	IntVec2 clientBounds = g_windowContext->GetTrueClientBounds();

	DebugRenderOptionsT options;
	const char* text = "Current Time %f";
	float currentTime = static_cast<float>(GetCurrentTimeSeconds());
	g_debugRenderer->DebugAddToLog(options, text, Rgba::YELLOW, 0.f, currentTime);

	text = "F5 to Toggle Material/Legacy mode";
	g_debugRenderer->DebugAddToLog(options, text, Rgba::WHITE, 0.f);
	
	text = "UP/DOWN to increase/decrease emissive factor";
	g_debugRenderer->DebugAddToLog(options, text, Rgba::WHITE, 0.f);

	//Update the camera's transform
	Matrix44 camTransform = Matrix44::MakeFromEuler( m_mainCamera->GetEuler(), m_rotationOrder ); 
	camTransform = Matrix44::SetTranslation3D(m_camPosition, camTransform);
	m_mainCamera->SetModelMatrix(camTransform);
	
	//RenderUsingMaterial();
	Ray3D ray = m_mainCamera->ScreenPointToWorldRay(intVecPos, clientBounds);
	//g_debugRenderer->DebugRenderLine(ray.m_start, ray.m_direction * 80.f, 0.0f);
	float out[2];
	
	uint pointCount = Raycast(out, ray, m_capsuleCPU);
	if (pointCount > 0U)
	{
		Vec3 hitPoint = ray.GetPointAtTime(out[0]);

		m_capsuleModel = Matrix44::IDENTITY;
		//m_cubeTransform = Matrix44::SetTranslation3D(hitPoint, m_cubeTransform);
		g_debugRenderer->DebugRenderCapsule(m_capsuleCPU, Vec3::ZERO, 0.f, m_textureTest);
	}
	else
	{
		g_debugRenderer->DebugRenderCapsule(m_capsuleCPU, Vec3::ZERO, 0.f, nullptr);
	}

	/*
	uint pointCount = Raycast(out, ray, m_sphereDebug);
	if (pointCount > 0U)
	{
		Vec3 hitPoint = ray.GetPointAtTime(out[0]);

		m_capsuleModel = Matrix44::IDENTITY;
		//m_cubeTransform = Matrix44::SetTranslation3D(hitPoint, m_cubeTransform);
		g_debugRenderer->DebugRenderSphere(m_sphereDebug.m_point, m_sphereDebug.m_radius, 0.f, m_textureTest);
	}
	else
	{
		g_debugRenderer->DebugRenderSphere(m_sphereDebug.m_point, m_sphereDebug.m_radius, 0.f, nullptr);
	}
	*/

	//Update the town center's matrix
	m_townCenterTransform = Matrix44::SetTranslation3D(m_RTSCam->m_focalPoint, m_townCenterTransform);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::ClearGarbageEntities()
{
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::CheckXboxInputs()
{
	//XboxController playerController = g_inputSystem->GetXboxController(0);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::CheckCollisions()
{		
}

//------------------------------------------------------------------------------------------------------------------------------
bool Game::HandleMouseLBDown()
{
	m_gameInput->HandleMouseLBDown();
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
bool Game::HandleMouseLBUp()
{
	m_gameInput->HandleMouseLBUp();
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
bool Game::HandleMouseRBDown()
{
	m_gameInput->HandleMouseRBDown();
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
bool Game::HandleMouseRBUp()
{
	m_gameInput->HandleMouseRBUp();
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
bool Game::HandleMouseScroll(float wheelDelta)
{
	m_gameInput->HandleMouseScroll(wheelDelta);
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::EnqueueCommand(RTSCommand *command)
{
	for (int index = 0; index < m_commandQueue.size(); index++)
	{
		if (m_commandQueue[index] == nullptr)
		{
			m_commandQueue[index] = command;
			return;
		}
	}

	m_commandQueue.push_back(command);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::ProcessCommands()
{	
	std::vector<RTSCommand*>::iterator itr;
	itr = m_commandQueue.begin();
	
	int numCommands = (int)m_commandQueue.size();

	for (int index = 0; index < numCommands; index++)
	{
		if (m_commandQueue[index] != nullptr)
		{
			m_commandQueue[index]->Execute();
			delete m_commandQueue[index];
			m_commandQueue[index] = nullptr;
		}
	}

	/*
	while (itr != m_commandQueue.end()) 
	{
		if (*itr != nullptr)
		{
			switch ((*itr)->m_commandType)
			{
			case CREATE_ENTITY: {} break;
			case MOVE_ENTITY: {} break;
			default:
				break;
			}

			(*itr)->Execute();
			delete (*itr);
			*itr = nullptr;
		}
		itr++;
	}
	*/
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::ClearCommands()
{
	m_commandQueue.clear();
}

//------------------------------------------------------------------------------------------------------------------------------
int Game::GetCurrentTeam() const
{
	return m_currentTeam;
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::SetCurrentTeam(int teamNumber)
{
	m_currentTeam = teamNumber;
}

//------------------------------------------------------------------------------------------------------------------------------
bool Game::IsAlive()
{
	//Check if alive
	return m_isGameAlive;
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::CreateMenuUIWidgets()
{
	// Menu Widgets
	m_menuParent = new UIWidget(this, nullptr);
	m_menuParent->SetColor(Rgba(0.f, 0.f, 0.f, 0.f));
	m_menuParent->UpdateBounds(AABB2(Vec2(0.f, 0.f), Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT, UI_SCREEN_HEIGHT)));

	//Create the radio group
	m_menuRadGroup = m_menuParent->CreateChild<UIRadioGroup>(m_menuParent->GetWorldBounds());

	AABB2 bounds = AABB2(Vec2(0.f, 0.f), Vec2(30.f, 30.f));
	Vec4 size = Vec4(0.1f, 0.1f, 0.f, 0.f);
	Vec4 position = Vec4(0.75f, 0.f, 0.f, 220.f);
	//Create the Play Button
	m_playButton = m_menuRadGroup->CreateChild<UIButton>(m_menuRadGroup->GetWorldBounds(), size, position);
	m_playButton->SetOnClick("GoToGame");
	m_playButton->SetColor(Rgba::WHITE);
	m_playButton->unHovercolor = Rgba::WHITE;
	m_playButton->SetRadioType(true);
	
	//Create the Edit button
	bounds = AABB2(Vec2(0.f, 0.f), Vec2(30.f, 30.f));
	size = Vec4(0.1f, 0.1f, 0.f, 0.f);
	position = Vec4(0.75f, 0.f, 0.f, 130.f);

	m_editButton = m_menuRadGroup->CreateChild<UIButton>(m_menuRadGroup->GetWorldBounds(), size, position);
	m_editButton->SetOnClick("GoToEdit");
	m_editButton->SetColor(Rgba::WHITE);
	m_editButton->unHovercolor = Rgba::WHITE;
	m_editButton->SetRadioType(true);

	size = Vec4(1.f, .75f, 0.f, 0.f);
	position = Vec4(0.5f, 0.5, 0.f, 0.f);
	
	UILabel* label = m_playButton->CreateChild<UILabel>(m_playButton->GetWorldBounds(), size, position);
	label->SetLabelText("PLAY");
	label->SetColor(Rgba::WHITE);

	label = m_editButton->CreateChild<UILabel>(m_editButton->GetWorldBounds(), size, position);
	label->SetLabelText("EDIT");
	label->SetColor(Rgba::WHITE);

	//Age of emptiness
	size = Vec4(0.4f, 0.45f, 0.f, 0.f);
	position = Vec4(0.35f, 1.0, 0.f, -120.f);

	label = m_menuParent->CreateChild<UILabel>(m_menuParent->GetWorldBounds(), size, position);
	label->SetLabelText("Age of Emptiness III");
	label->SetColor(Rgba(0.f, 0.f, 0.f, 1.f));

	size = Vec4(0.4f, 0.4f, 0.f, 0.f);
	position = Vec4(0.35f, 1.0, 0.f, -117.f);

	label = m_menuParent->CreateChild<UILabel>(m_menuParent->GetWorldBounds(), size, position);
	label->SetLabelText("Age of Emptiness III");
	label->SetColor(Rgba::WHITE);

	size = Vec4(0.35f, 0.35f, 0.f, 0.f);
	position = Vec4(0.35f, 1.0, 0.f, -160.f);

	label = m_menuParent->CreateChild<UILabel>(m_menuParent->GetWorldBounds(), size, position);
	label->SetLabelText("A truly empty 3D experience");
	label->SetColor(Rgba::DARK_GREY);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::CreateEditUIWidgets()
{
	// Menu Widgets
	m_editParent = new UIWidget(this, nullptr);
	m_editParent->SetColor(Rgba(0.f, 0.f, 0.f, 0.f));

	IntVec2 client = g_windowContext->GetTrueClientBounds();

	Vec2 boundsSize = Vec2((float)client.x, (float)client.y); 
	m_editParent->UpdateBounds(AABB2(Vec2::ZERO, Vec2(UI_SCREEN_HEIGHT * UI_SCREEN_ASPECT, UI_SCREEN_HEIGHT)));

	//Create the radio group
	m_editRadGroup = m_editParent->CreateChild<UIRadioGroup>(m_editParent->GetWorldBounds());

	Vec4 size = Vec4( 0.0f, 0.0f, 64.0f, 64.0f );
	Vec4 position = Vec4(0.f, 1.f, 250.f, -150.f);;

	//Create the first Button
	UIButton* button = m_editRadGroup->CreateChild<UIButton>(m_editRadGroup->GetWorldBounds(), size, position);
	button->SetOnClick("isActive=true");
	button->SetColor(Rgba::WHITE);
	button->SetButtonTexture("stone_diffuse.png");
	button->unHovercolor = Rgba::WHITE;
	button->hoverColor = Rgba::GREEN;
	button->SetRadioType(true);

	//Create the 2nd button
	position = Vec4(0.f, 1.f, 350.f, -150.f);

	button = m_editRadGroup->CreateChild<UIButton>(m_editRadGroup->GetWorldBounds(), size, position);
	button->SetOnClick("isActive=true");
	button->SetColor(Rgba::WHITE);
	button->SetButtonTexture("stone_normal.png");
	button->SetRadioType(true);
	button->unHovercolor = Rgba::WHITE;
	button->hoverColor = Rgba::GREEN;

	//Create the 3rd button
	position = Vec4(0.f, 1.f, 450.f, -150.f);

	button = m_editRadGroup->CreateChild<UIButton>(m_editRadGroup->GetWorldBounds(), size, position);
	button->SetOnClick("isActive=true");
	button->SetColor(Rgba::WHITE);
	button->SetButtonTexture("stone_spec.png");
	button->SetRadioType(true);
	button->unHovercolor = Rgba::WHITE;
	button->hoverColor = Rgba::GREEN;

	//Create the 4th button
	position = Vec4(0.f, 1.f, 550.f, -150.f);

	button = m_editRadGroup->CreateChild<UIButton>(m_editRadGroup->GetWorldBounds(), size, position);
	button->SetOnClick("isActive=true");
	button->SetColor(Rgba::WHITE);
	button->SetButtonTexture("Test_StbiFlippedAndOpenGL.png");
	button->SetRadioType(true);
	button->unHovercolor = Rgba::WHITE;
	button->hoverColor = Rgba::GREEN;

}

//------------------------------------------------------------------------------------------------------------------------------
void Game::CreateGameUIWidgets()
{
	/*
	// Menu Widgets
	m_gameParent = new UIWidget(this, nullptr);
	m_gameParent->SetColor(Rgba(0.f, 0.f, 0.f, 0.f));
	m_gameParent->UpdateBounds(AABB2(Vec2(0.f, 0.f), Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT, UI_SCREEN_HEIGHT)));

	/*
	//Init
	Vec4 size = Vec4(0.4f, 0.3f, 0.f, 0.f);
	Vec4 position = Vec4(0.4f, 1.0, 0.f, -120.f);

	UILabel* label = m_gameParent->CreateChild<UILabel>(m_gameParent->GetWorldBounds(), size, position);
	label->SetLabelText("NUM_1: Go to init state");
	label->SetColor(Rgba::WHITE);
	*/
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::CreatePauseUIWidgets()
{
	// Menu Widgets
	m_pauseParent = new UIWidget(this, nullptr);
	m_pauseParent->SetColor(Rgba(0.f, 0.f, 0.f, 0.f));
	m_pauseParent->UpdateBounds(AABB2(Vec2(0.f, 0.f), Vec2(UI_SCREEN_ASPECT * UI_SCREEN_HEIGHT, UI_SCREEN_HEIGHT)));

	//Create the radio group
	m_pauseRadGroup = m_pauseParent->CreateChild<UIRadioGroup>(m_pauseParent->GetWorldBounds());

	AABB2 bounds = AABB2(Vec2(0.f, 0.f), Vec2(30.f, 30.f));
	Vec4 size = Vec4(0.1f, 0.1f, 0.f, 0.f);
	Vec4 position = Vec4(0.5f, 0.5f, -150.f, -120.f);
	//Create the Resume Button
	m_resumeButton = m_pauseRadGroup->CreateChild<UIButton>(m_pauseRadGroup->GetWorldBounds(), size, position);
	m_resumeButton->SetOnClick("ResumeGame");
	m_resumeButton->SetColor(Rgba::WHITE);
	m_resumeButton->unHovercolor = Rgba::WHITE;
	m_resumeButton->SetRadioType(true);

	//Create the Menu button
	bounds = AABB2(Vec2(0.f, 0.f), Vec2(30.f, 30.f));
	size = Vec4(0.1f, 0.1f, 0.f, 0.f);
	position = Vec4(0.5f, 0.5f, 0.f, -120.f);

	m_returnMenuButton = m_pauseRadGroup->CreateChild<UIButton>(m_pauseRadGroup->GetWorldBounds(), size, position);
	m_returnMenuButton->SetOnClick("ReturnToMenu");
	m_returnMenuButton->SetColor(Rgba::WHITE);
	m_returnMenuButton->unHovercolor = Rgba::WHITE;
	m_returnMenuButton->SetRadioType(true);

	//Create the Menu button
	bounds = AABB2(Vec2(0.f, 0.f), Vec2(30.f, 30.f));
	size = Vec4(0.1f, 0.1f, 0.f, 0.f);
	position = Vec4(0.5f, 0.5f, 150.f, -120.f);

	m_quitButton = m_pauseRadGroup->CreateChild<UIButton>(m_pauseRadGroup->GetWorldBounds(), size, position);
	m_quitButton->SetOnClick("QuitGame");
	m_quitButton->SetColor(Rgba::WHITE);
	m_quitButton->unHovercolor = Rgba::WHITE;
	m_quitButton->SetRadioType(true);

	size = Vec4(1.f, .75f, 0.f, 0.f);
	position = Vec4(0.5f, 0.5, 0.f, 0.f);

	UILabel* label = m_resumeButton->CreateChild<UILabel>(m_resumeButton->GetWorldBounds(), size, position);
	label->SetLabelText("RESUME");
	label->SetColor(Rgba::WHITE);

	label = m_returnMenuButton->CreateChild<UILabel>(m_returnMenuButton->GetWorldBounds(), size, position);
	label->SetLabelText("MENU");
	label->SetColor(Rgba::WHITE);

	label = m_quitButton->CreateChild<UILabel>(m_quitButton->GetWorldBounds(), size, position);
	label->SetLabelText("QUIT");
	label->SetColor(Rgba::WHITE);

	//Age of emptiness
	size = Vec4(0.4f, 0.45f, 0.f, 0.f);
	position = Vec4(0.5f, 0.5, 0.f, 0.f);

	label = m_pauseParent->CreateChild<UILabel>(m_pauseParent->GetWorldBounds(), size, position);
	label->SetLabelText("GAME PAUSED");
	label->SetColor(Rgba::YELLOW);

	size = Vec4(0.4f, 0.4f, 0.f, 0.f);
	position = Vec4(0.5f, 0.5f, 0.f, 0.f);

	label = m_pauseParent->CreateChild<UILabel>(m_pauseParent->GetWorldBounds(), size, position);
	label->SetLabelText("GAME PAUSED");
	label->SetColor(Rgba::ORANGE);

	size = Vec4(0.35f, 0.35f, 0.f, 0.f);
	position = Vec4(0.5, 0.5, 0.f, -50.f);

	label = m_pauseParent->CreateChild<UILabel>(m_pauseParent->GetWorldBounds(), size, position);
	label->SetLabelText("Pronay->SetJoy( currentMapVisibility );");
	label->SetColor(Rgba::DARK_GREY);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::LoadGameMaterials()
{
	m_testMaterial = g_renderContext->CreateOrGetMaterialFromFile(m_materialPath);

	m_toneMap = g_renderContext->CreateOrGetMaterialFromFile(m_tonemapPath);
	TonemapBufferT buffer;
	buffer.intensity = 1.f;

	m_toneMap->SetUniforms(&buffer, sizeof(buffer));
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::UpdateLightPositions()
{
	//Update all the 4 light positions
	float currentTime = static_cast<float>(GetCurrentTimeSeconds());
	
	//DebugRenderOptionsT options;
	//options.space = DEBUG_RENDER_WORLD;
	
	//Light 1
	m_dynamicLight0Pos = Vec3(-3.f, 2.f * CosDegrees(currentTime * 20.f), 2.f * SinDegrees(currentTime * 20.f));

	g_renderContext->m_cpuLightBuffer.lights[1].position = m_dynamicLight0Pos;
	g_renderContext->m_lightBufferDirty = true;

	//options.beginColor = Rgba::GREEN;
	//options.endColor = Rgba::GREEN * 0.4f;
	//g_debugRenderer->DebugRenderPoint(options, m_dynamicLight0Pos, 0.1f, 0.1f, nullptr);

	//Light 2
	m_dynamicLight1Pos = Vec3(3.f, 3.f * CosDegrees(currentTime * 40.f), 3.f * SinDegrees(currentTime * 40.f));
	g_renderContext->m_cpuLightBuffer.lights[2].position = m_dynamicLight1Pos;
	g_renderContext->m_lightBufferDirty = true;

	//options.beginColor = Rgba::BLUE;
	//options.endColor = Rgba::BLUE * 0.4f;
	//g_debugRenderer->DebugRenderPoint(options, m_dynamicLight1Pos, 0.1f, 0.1f, nullptr);

	//Light 3
	m_dynamicLight2Pos = Vec3(-1.f, 1.f * CosDegrees(currentTime * 30.f), 1.f * SinDegrees(currentTime * 30.f));
	g_renderContext->m_cpuLightBuffer.lights[3].position = m_dynamicLight2Pos;
	g_renderContext->m_lightBufferDirty = true;

	//options.beginColor = Rgba::YELLOW;
	//options.endColor = Rgba::YELLOW * 0.4f;
	//g_debugRenderer->DebugRenderPoint(options, m_dynamicLight2Pos, 0.1f, 0.1f, nullptr);

	//Light 4
	m_dynamicLight3Pos = Vec3(4.f * CosDegrees(currentTime * 60.f), 0.f , 4.f * SinDegrees(currentTime * 60.f));
	g_renderContext->m_cpuLightBuffer.lights[4].position = m_dynamicLight3Pos;
	g_renderContext->m_lightBufferDirty = true;

	//options.beginColor = Rgba::MAGENTA;
	//options.endColor = Rgba::MAGENTA * 0.4f;
	//g_debugRenderer->DebugRenderPoint(options, m_dynamicLight3Pos, 0.1f, 0.1f, nullptr);

}

//------------------------------------------------------------------------------------------------------------------------------
void Game::CreateInitialLight()
{
	g_renderContext->EnableDirectionalLight(Vec3(1.f, 1.f, 1.f), Vec3(0.f, 0.f, 1.f), Rgba::DARK_GREY, 0.f);

	g_renderContext->EnablePointLight(1U, m_dynamicLight0Pos, Vec3(1.f, 0.f, 0.5f),Rgba::GREEN, 0.f);
	g_renderContext->EnablePointLight(2U, m_dynamicLight1Pos, Vec3(0.f, -1.f, 0.f), Rgba::BLUE, 0.f, Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.1f, 0.f));
	g_renderContext->EnablePointLight(3U, m_dynamicLight2Pos, Vec3(0.f, 0.f, 1.f), Rgba::YELLOW, 0.f, Vec3(0.f, 1.f, 0.1f), Vec3(0.f, 0.1f, 0.f));
	g_renderContext->EnablePointLight(4U, m_dynamicLight3Pos, Vec3(-1.f, -1.f, 0.f), Rgba::MAGENTA, 0.f, Vec3(0.f, 0.f, 1.f), Vec3(0.f, 0.f, 1.f));
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::LoadInitMesh()
{
	m_lastState = STATE_LOAD;
	if (m_initMesh == nullptr)
	{
		m_initMesh = new Model(g_renderContext, m_objectPath);
	}
	m_lastState = m_gameState;
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::BeginFrame()
{
	if (m_returnToMenu == true)
	{
		m_pauseTimer = 0.f;
		m_gameState = STATE_MENU;
	}

	m_gameInput->BeginFrame();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::EndFrame()
{
	m_gameInput->EndFrame();
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::CreateInitialMeshes()
{

	//Meshes for A4
	CPUMesh mesh;
	CPUMeshAddQuad(&mesh, AABB2(Vec2(-0.5f, -0.5f), Vec2(0.5f, 0.5f)));
	m_quad = new GPUMesh(g_renderContext);
	m_quad->CreateFromCPUMesh<Vertex_Lit>(&mesh, GPU_MEMORY_USAGE_STATIC);

	// create a cube (centered at zero, with sides 2 length)
	CPUMeshAddCube( &mesh, AABB3( Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, 0.5f)) ); 
	
	//mesh.SetLayout<Vertex_Lit>();
	m_cube = new GPUMesh( g_renderContext ); 
	m_cube->CreateFromCPUMesh<Vertex_Lit>( &mesh, GPU_MEMORY_USAGE_STATIC );


	// create a sphere, cenetered at zero, with 
	mesh.Clear();
	CPUMeshAddUVSphere( &mesh, Vec3::ZERO, 1.0f );  
	
	//mesh.SetLayout<Vertex_Lit>();
	m_sphere = new GPUMesh( g_renderContext ); 
	m_sphere->CreateFromCPUMesh<Vertex_Lit>( &mesh, GPU_MEMORY_USAGE_STATIC );

	//Create another quad as a base plane
	mesh.Clear();
	CPUMeshAddQuad(&mesh, AABB2(Vec2(-50.f, -50.f), Vec2(50.f, 50.f)));

	//mesh.SetLayout<Vertex_Lit>();
	m_baseQuad = new GPUMesh( g_renderContext ); 
	m_baseQuad->CreateFromCPUMesh<Vertex_Lit>( &mesh, GPU_MEMORY_USAGE_STATIC );

	m_baseQuadTransform = Matrix44::IDENTITY;
	m_baseQuadTransform = Matrix44::MakeFromEuler(Vec3(-90.f, 0.f, 0.f));
	m_baseQuadTransform = Matrix44::SetTranslation3D(Vec3(0.f, -1.f, 0.f), m_baseQuadTransform);

	mesh.Clear();
	CPUMeshAddUVCapsule(&mesh, Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.f, 0.f), 0.5f, Rgba::YELLOW);

	m_capsule = new GPUMesh(g_renderContext);
	m_capsule->CreateFromCPUMesh<Vertex_Lit>(&mesh, GPU_MEMORY_USAGE_STATIC);

	m_capsuleModel = Matrix44::IDENTITY;
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::LoadGameTextures()
{
	//Get the test texture
	m_textureTest = g_renderContext->CreateOrGetTextureViewFromFile(m_testImagePath);
	m_boxTexture = g_renderContext->CreateOrGetTextureViewFromFile(m_boxTexturePath);
	m_sphereTexture = g_renderContext->CreateOrGetTextureViewFromFile(m_sphereTexturePath);
	m_backgroundTexture = g_renderContext->CreateOrGetTextureViewFromFile(m_backgroundPath);

	m_peonTexture = g_renderContext->CreateOrGetTextureViewFromFile(m_peonSheetPath);
	m_warriorTexture = g_renderContext->CreateOrGetTextureViewFromFile(m_warriorSheetPath);
	m_peonAttackTexture = g_renderContext->CreateOrGetTextureViewFromFile(m_peonAttackSheetPath);
	m_warriorAttackTexture = g_renderContext->CreateOrGetTextureViewFromFile(m_warriorAttackSheetPath);


	m_peonSheet = new SpriteSheet(m_peonTexture, m_peonSheetDim);
	m_warriorSheet = new SpriteSheet(m_warriorTexture, m_warriorSheetDim);
	m_peonAttackSheet = new SpriteSheet(m_peonAttackTexture, m_peonAttackSheetDim);
	m_warriorAttackSheet = new SpriteSheet(m_warriorAttackTexture, m_warriorAttackSheetDim);

	CreateIsoSpriteDefenitions();
}

void Game::CreateIsoSpriteDefenitions()
{
	//use this spot to create an isoSpriteDefenition
	std::vector<SpriteDefenition> spriteDefs;

	spriteDefs.push_back(SpriteDefenition(m_peonSheet->GetSpriteDef(0), Vec2(0.5, 0.25)));
	spriteDefs.push_back(SpriteDefenition(m_peonSheet->GetSpriteDef(8), Vec2(0.5, 0.25)));
	spriteDefs.push_back(SpriteDefenition(m_peonSheet->GetSpriteDef(16), Vec2(0.5, 0.25)));
	spriteDefs.push_back(SpriteDefenition(m_peonSheet->GetSpriteDef(24), Vec2(0.5, 0.25)));
	spriteDefs.push_back(SpriteDefenition(m_peonSheet->GetSpriteDef(32), Vec2(0.5, 0.25)));
	spriteDefs.push_back(SpriteDefenition(m_peonSheet->GetSpriteDef(40), Vec2(0.5, 0.25)));
	spriteDefs.push_back(SpriteDefenition(m_peonSheet->GetSpriteDef(48), Vec2(0.5, 0.25)));
	spriteDefs.push_back(SpriteDefenition(m_peonSheet->GetSpriteDef(56), Vec2(0.5, 0.25)));

	//m_isoSprite = new IsoSpriteDefenition(&spriteDefs[0], 8);
}

//------------------------------------------------------------------------------------------------------------------------------
void Game::GetandSetShaders()
{
	//Get the Shaders
	m_normalShader = g_renderContext->CreateOrGetShaderFromFile(m_normalColorShader);
	m_normalShader->SetDepth(eCompareOp::COMPARE_LEQUAL, true);

	m_defaultLit = g_renderContext->CreateOrGetShaderFromFile(m_shaderLitPath);
	m_defaultLit->SetDepth(eCompareOp::COMPARE_LEQUAL, true);
}

//------------------------------------------------------------------------------------------------------------------------------
// Forseth Mouse Update code
//------------------------------------------------------------------------------------------------------------------------------
void Game::UpdateMouseInputs(float deltaTime)
{
	//Get pitch and yaw from mouse
	IntVec2 mouseRelativePos = g_windowContext->GetClientMouseRelativeMovement();
	Vec2 mouse = Vec2((float)mouseRelativePos.x, (float)mouseRelativePos.y);

	// we usually want to scale the pixels so we can think of it
	// as a rotational velocity;  Work with these numbers until 
	// it feels good to you; 
	Vec2 scalingFactor = Vec2( 10.f, 10.f ); 
	Vec2 turnSpeed = mouse * scalingFactor; 

	// y mouse movement would correspond to rotation around right (x for us)
	// and x mouse movement corresponds with movement around up (y for us)
	Vec3 camEuler = m_mainCamera->GetEuler();
	camEuler -= deltaTime * Vec3( turnSpeed.y, turnSpeed.x, 0.0f ); 
	m_mainCamera->SetEuler(camEuler);

	// Let's fix our "pitch", or rotation around right to be limited to -85 to 85 degrees (no going upside down)
	camEuler.x = Clamp( camEuler.x, -85.0f, 85.0f );

	// Next, let's keep the turning as between 0 and 360 (some people prefer -180.0f to 180.0f)
	// either way, we just want to keep it a single revolution
	// Note: modf does not correctly mod negative numbers (it'll ignore the sign and mod them as if 
	// they were positive), so I write a special mod function to take this into account;  
	//m_camEuler.y = Modf( m_camEuler.y, 360.0f ); 

	// Awesome, I now have my euler, let's construct a matrix for it;
	// this gives us our current camera's orientation.  we will 
	// use this to translate our local movement to a world movement 
	Matrix44 camMatrix = Matrix44::MakeFromEuler( camEuler ); 

}
