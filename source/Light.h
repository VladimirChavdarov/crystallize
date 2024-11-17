#pragma once
#include "Camera.h"

enum TYPE
{
	POINT_LIGHT,
	SPOT_LIGHT
};

class Light
{
public:
	Light(float2 pos, float3 color, int type, int id);
	void Update(const Camera& cam);
	void Draw(const int& x, const int& y, const float2& mousePos, float3& color);

	int m_id;
	float2 m_pos;
	float2 m_screenPos = { 0.0f, 0.0f };
	float m_intensity = 1.0f;
	float3 m_color;
	int m_type = 0;
	float m_maxRadius;
};

class PointLight : Light
{
public:
	PointLight(float2 pos, float3 color);
	float3 ColorAtPixel(const int& x, const int& y);
};

class SpotLight : Light
{
public:
	SpotLight(float2 pos, float3 color);
	float3 ColorAtPixel(const int& x, const int& y, const float2& normLightDir);

	float2 normLightDir;
};