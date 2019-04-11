//------------------------------------------------------------------------------------------------------------------------------
#include "Game/RTSCamera.hpp"

//Engine Systems
#include "Engine/Math/MathUtils.hpp"

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
void RTSCamera::SetZoomDelta( float delta )
{
	m_currentZoom += delta;
	m_distance = RangeMapFloat(m_currentZoom, 0.f, 1.f, m_minDistance, m_maxDistance);
}

