//------------------------------------------------------------------------------------------------------------------------------
#include "Game/RTSCamera.hpp"

//Engine Systems
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/MathUtils.hpp"

//Game Systems
#include "Game/GameCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
RTSCamera::RTSCamera()
	: Camera()
{
	m_tilt = m_defaultTilt;
	m_angle = m_defaultAngle;
	m_focalPoint = Vec3(3.5f, 3.5f, 0.f);
	m_currentZoom = RangeMapFloat(m_distance, m_minDistance, m_maxDistance, 0.f, 1.f);
}

RTSCamera::~RTSCamera()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void RTSCamera::Update( float deltaTime )
{
	UNUSED(deltaTime);

	m_tilt = RangeMapFloat(m_distance, m_maxDistance, m_minDistance, m_tiltBounds.y, m_tiltBounds.x);

	//We are at an angle of m_angle around y axis
	//We are at a tilt of m_tilt around x axis
	float height = SinDegrees(m_tilt) * m_distance;
	float xyDistance = CosDegrees(m_tilt) * m_distance;

	Vec3 positionOffset = Vec3(CosDegrees(m_angle), SinDegrees(m_angle), 0.f) * xyDistance + Vec3(0.f, 0.f, -height);

	//What is our forward vector (What am I looking at?)
	Vec3 camPosition = m_focalPoint + positionOffset;

	//Give me a model matrix of something at offset looking at the focal point. This is now my model matrix
	m_modelMatrix = Matrix44::LookAt(camPosition, m_focalPoint, Vec3(0.f, 0.f, -1.f));
	SetModelMatrix(m_modelMatrix);
}

//------------------------------------------------------------------------------------------------------------------------------
void RTSCamera::SetFocusBounds( const AABB2& bounds )
{
	m_FPBounds = bounds;
}

//------------------------------------------------------------------------------------------------------------------------------
void RTSCamera::SetFocalPoint( Vec3 const &pos )
{
	m_focalPoint = pos;
}

//------------------------------------------------------------------------------------------------------------------------------
void RTSCamera::SetZoom( float zoom )
{
	m_currentZoom = zoom;
	m_distance = RangeMapFloat(m_currentZoom, 0.f, 1.f, m_minDistance, m_maxDistance);
}

//------------------------------------------------------------------------------------------------------------------------------
void RTSCamera::SetAngleOffset( float angleOffset )
{
	m_angle += angleOffset;
}

//------------------------------------------------------------------------------------------------------------------------------
void RTSCamera::PanFocalPoint( Vec3 panAmount )
{
	m_focalPoint += panAmount;
	m_focalPoint.ClampVector(m_focalPoint, m_FPBounds.m_minBounds, m_FPBounds.m_maxBounds);
}

//------------------------------------------------------------------------------------------------------------------------------
void RTSCamera::SetZoomDelta( float delta )
{
	m_currentZoom += delta;
	m_distance = RangeMapFloat(m_currentZoom, MIN_ZOOM_STEPS, MAX_ZOOM_STEPS, m_minDistance, m_maxDistance);

	m_distance = Clamp(m_distance, m_minDistance, m_maxDistance);
}

