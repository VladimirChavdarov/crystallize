#pragma once

class Camera
{
public:
	Camera(float2 coord, float scale, float angle);

	void Reset();
	void UpdateCamera(const float& deltaTime, const float2& mousePos);

	float2 m_startCoord;
	float2 m_coord;
	float m_startScale;
	float m_scale;
	float m_startAngle;
	float m_angle;

	float2 A = { 1.0f, 1.0f };
};

