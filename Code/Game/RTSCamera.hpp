//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vec3.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class RTSCamera : public Camera
{
public:
	RTSCamera();
	~RTSCamera();

	// Called each frame to update the underlying camera with the RTSCamera's options; 
	void Update( float deltaTime ); 

	void SetFocusBounds( const AABB2& bounds);

	void SetFocalPoint( Vec3 const &pos ); 
	void SetZoom( float zoom ); //Manipulates distance
	void SetAngle( float angleOffset ); // really is setting an angle offset

	void PanFocalPoint( Vec3 panAmount );
	void SetZoomDelta( float delta );

public:
	Vec3 m_focalPoint; 
	float m_distance = 5.f; 

	float m_currentZoom = 0.f;

	// configuration - mess with these numbers to get a view you like; 
	float m_minDistance     = 2.0f;
	float m_maxDistance     = 16.0f;

	float m_defaultAngle    = -135.0f; 
	float m_defaultTilt     = 40.0f; 
	
	Vec2 m_tiltBounds		= Vec2(10.f, 40.f);

	// used for moving from the default
	float m_angleOffset     = 0.0f; 

	Matrix44 m_modelMatrix	= Matrix44::IDENTITY;

	//The actual tilt and angle for the camera
	float m_tilt;
	float m_angle;

	AABB2 m_FPBounds;	//Bounds for the focal point

	// juice
	// ...to make a smoother camera, you may want to save off "targets" in which
	// to trend toward over multiple frames.  Not required, but that sort of logic I think
	// lives comfortably on the camera

}; 
