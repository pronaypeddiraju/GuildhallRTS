//------------------------------------------------------------------------------------------------------------------------------
#include "Game/App.hpp"
//Engine Systems
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystems.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/XMLUtils/XMLUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
//Game Systems
#include "Game/Game.hpp"

App* g_theApp = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
App::App()
{	
}

//------------------------------------------------------------------------------------------------------------------------------
App::~App()
{
	ShutDown();
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool App::Command_Quit(EventArgs& args)
{
	UNUSED(args);
	g_theApp->HandleQuitRequested();
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
void App::LoadGameBlackBoard()
{
	const char* xmlDocPath = "Data/Gameplay/GameConfig.xml";
	tinyxml2::XMLDocument gameconfig;
	gameconfig.LoadFile(xmlDocPath);
	
	if(gameconfig.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		printf("\n >> Error loading XML file from %s ", xmlDocPath);
		printf("\n >> Error ID : %i ", gameconfig.ErrorID());
		printf("\n >> Error line number is : %i", gameconfig.ErrorLineNum());
		printf("\n >> Error name : %s", gameconfig.ErrorName());
		ERROR_AND_DIE(">> Error loading GameConfig XML file ")
		return;
	}
	else
	{
		//We read everything fine. Now just shove all that data into the black board
		XMLElement* rootElement = gameconfig.RootElement();
		g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*rootElement);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void App::StartUp()
{
	LoadGameBlackBoard();

	g_eventSystem = new EventSystems();

	//This is now being set in Main_Windows.cpp
	//g_renderContext = new RenderContext(m_appWindowHandle);
	
	g_inputSystem = new InputSystem();

	g_audio = new AudioSystem();

	g_devConsole = new DevConsole();
	g_devConsole->Startup();

	//create the networking system
	//g_networkSystem = new NetworkSystem();

	g_debugRenderer = new DebugRender();
	g_debugRenderer->Startup(g_renderContext);

	g_RNG = new RandomNumberGenerator();

	m_game = new Game();
	m_game->StartUp();
	
	g_eventSystem->SubscribeEventCallBackFn("Quit", Command_Quit);
}

//------------------------------------------------------------------------------------------------------------------------------
void App::ShutDown()
{
	delete g_renderContext;
	g_renderContext = nullptr;

	delete g_inputSystem;
	g_inputSystem = nullptr;

	delete g_audio;
	g_audio = nullptr;

	delete g_devConsole;
	g_devConsole = nullptr;

	delete g_eventSystem;
	g_eventSystem = nullptr;

	delete g_debugRenderer;
	g_debugRenderer = nullptr;

	delete g_RNG;
	g_RNG = nullptr;

	m_game->Shutdown();

	uint totalAllocs = (uint)gTotalAllocations;
	uint totalAllocBytes = (uint)gTotalBytesAllocated;

	DebuggerPrintf("Memory Allocations total: %u, Bytes Allocated total: %u \n", totalAllocs, totalAllocBytes);
}

//------------------------------------------------------------------------------------------------------------------------------
void App::RunFrame()
{
	BeginFrame();	
	
	Update();
	Render();	

	PostRender();

	EndFrame();
}

//------------------------------------------------------------------------------------------------------------------------------
void App::BeginFrame()
{
	uint thisFrame = (uint)gAllocatedThisFrame;
	uint thisFrameBytes = (uint)gAllocatedBytesThisFrame;

	DebuggerPrintf("Memory Allocations this Frame: %u, Bytes Allocated this Frame: %u \n", thisFrame, thisFrameBytes);

	gAllocatedThisFrame = 0U;
	gAllocatedBytesThisFrame = 0U;

	g_renderContext->BeginFrame();
	g_inputSystem->BeginFrame();
	g_audio->BeginFrame();
	g_devConsole->BeginFrame();
	g_eventSystem->BeginFrame();
	g_debugRenderer->BeginFrame();

	m_game->BeginFrame();
}

//------------------------------------------------------------------------------------------------------------------------------
void App::EndFrame()
{
	g_renderContext->EndFrame();
	g_inputSystem->EndFrame();
	g_audio->EndFrame();
	g_devConsole->EndFrame();
	g_eventSystem->EndFrame();
	g_debugRenderer->EndFrame();

	m_game->EndFrame();
}

//------------------------------------------------------------------------------------------------------------------------------
void App::Update()
{	
	m_timeAtLastFrameBegin = m_timeAtThisFrameBegin;
	m_timeAtThisFrameBegin = GetCurrentTimeSeconds();

	float deltaTime = static_cast<float>(m_timeAtThisFrameBegin - m_timeAtLastFrameBegin);
	deltaTime = Clamp(deltaTime, 0.0f, 0.1f);

	g_devConsole->UpdateConsole(deltaTime);

	m_game->Update(deltaTime);

	g_debugRenderer->Update(deltaTime);
}

//------------------------------------------------------------------------------------------------------------------------------
void App::Render() const
{
	m_game->Render();
}

//------------------------------------------------------------------------------------------------------------------------------
void App::PostRender()
{
	m_game->PostRender();
}

//------------------------------------------------------------------------------------------------------------------------------
bool App::HandleKeyPressed(unsigned char keyCode)
{
	if(keyCode == TILDY_KEY)
	{
		g_devConsole->ToggleOpenFull();
	}

	switch(keyCode)
	{
		case F8_KEY:
		{
			//Kill and restart the app
			delete m_game;
			m_game = nullptr;
			m_game = new Game();
			m_game->StartUp();
			return true;
		}
		case KEY_ESC:
		{
			if(!g_devConsole->IsOpen())
			{
				//Shut the app
				g_theApp->HandleQuitRequested();
				return true;
			}
			else
			{
				m_game->HandleKeyPressed(keyCode);
				return true;
			}
		}
		default:
		{
			m_game->HandleKeyPressed(keyCode);
			return true;
		}
		break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
bool App::HandleKeyReleased(unsigned char keyCode)
{
	m_game->HandleKeyReleased(keyCode);
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
bool App::HandleCharacter( unsigned char charCode )
{
	m_game->HandleCharacter(charCode);
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------
bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return m_isQuitting;
}

//------------------------------------------------------------------------------------------------------------------------------
bool App::HandleMouseLBDown()
{
	//Implement Mouse Left button down logic here
	return m_game->HandleMouseLBDown();
}

//------------------------------------------------------------------------------------------------------------------------------
bool App::HandleMouseLBUp()
{
	//Implement Mouse Left button Up logic here
	return m_game->HandleMouseLBUp();
}

//------------------------------------------------------------------------------------------------------------------------------
bool App::HandleMouseRBDown()
{
	//Implement Mouse Right Button Down logic here
	return m_game->HandleMouseRBDown();
}

//------------------------------------------------------------------------------------------------------------------------------
bool App::HandleMouseRBUp()
{
	//Implement Mouse Right Button Up logic here
	return m_game->HandleMouseRBUp();	
}

//------------------------------------------------------------------------------------------------------------------------------
bool App::HandleMouseScroll(float wheelDelta)
{
	return m_game->HandleMouseScroll(wheelDelta);
}