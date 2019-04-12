//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

//Game Systems
#include "Game/Game.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class GameInput
{
public:
	GameInput(Game* game);
	~GameInput();
	// Called each frame - translates raw input (keyboard/mouse/etc) to 
	// input used for the game.
	// 
	// Hotkey bindings will also live here; 
	void								BeginFrame();
	void								Update(float deltaTime); 
	void								EndFrame();

	// A08
	Vec2								GetFramePan() const; 
	float								GetFrameZoom() const; 

	float								GetFrameRotation() const;     
	bool								IsRotating() const;            // are we in a rotation mode (Control is Held)

	void								SetFramePan(Vec2 panAmount);


	//Mouse Input handling
	bool								HandleMouseLBDown();
	bool								HandleMouseLBUp();
	bool								HandleMouseRBDown();
	bool								HandleMouseRBUp();
	bool								HandleMouseScroll(float wheelDelta);

	//Key input handling
	void								HandleKeyPressed( unsigned char keyCode );
	void								HandleKeyReleased( unsigned char keyCode );

	// A09
	// eGameAction DequeueNextAction(); 

public:
	// Configuration - Keyboard Input
	float m_keyboardPanSpeed         = 10.0f; 

	// Configuration - Mouse Input
	float m_edgePanSpeed             = 16.0f; // how quickly we pan when using edge-scroll
	float m_edgePanDistance          = 32.0f; // units away from the edge in 2D before scrolling starts

	float m_rotationSpeed            = PI; 
	float m_zoomSpeed                = 24.0f; 
	const float m_maxZoom			 = 100.f;

	// EXTRA:  Support Middle Mouse Button "Drag Pan"
	Game* m_game					 = nullptr;

	Vec2 m_framePan					 = Vec2::ONE;
	float m_frameZoomDelta			 = 0.f;
	float m_frameRotation			 = 0.f;
	float m_frameZoom				 = 0.f;

	bool m_isRotating				 = false;

	bool m_APressed					 = false;
	bool m_WPressed					 = false;
	bool m_SPressed					 = false;
	bool m_DPressed					 = false;

}; 