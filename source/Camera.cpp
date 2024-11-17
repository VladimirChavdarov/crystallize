#include "precomp.h"
#include "Camera.h"

Camera::Camera(float2 coord, float scale, float angle)
{
	m_startCoord = coord;
	m_coord = coord;
	m_startScale = scale;
	m_scale = scale;
	m_startAngle = angle;
	m_angle = angle;

	A = { cosf(DEG_TO_RAD(m_angle)), sinf(DEG_TO_RAD(m_angle)) };
}

void Camera::Reset()
{
	m_coord = m_startCoord;
	m_scale = m_startScale;
	m_angle = m_startAngle;
}

void Camera::UpdateCamera(const float& deltaTime, const float2& mousePos)
{
	
}