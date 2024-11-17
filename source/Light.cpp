#include "precomp.h"
#include "Light.h"

Light::Light(float2 pos, float3 color, int type, int id)
{
	m_id = id;
	m_pos.x = pos.x;
	m_pos.y = pos.y;
	m_color = color;
	m_type = type;
	
	float max = fmaxf(color.x, fmaxf(color.y, color.z));
	float colorPercent = 0.001f;
	m_maxRadius = sqrtf(max / colorPercent) * 10.0f;
}

// 5-6 less FPS when I abstract it in a function
void Light::Draw(const int& x, const int& y, const float2& mousePos, float3& color)
{
	float2 vec = float2(m_screenPos.x - x, m_screenPos.y - y);
	float radius = length(vec) / m_intensity;
	// spot lights
	if (m_type == 1)
	{
		//float2 lightDir = float2(lights[i]->m_screenPos.x - lights[0]->m_screenPos.x, lights[i]->m_screenPos.y - lights[0]->m_screenPos.y);
		float2 lightDir = float2(m_screenPos.x - mousePos.x, m_screenPos.y - mousePos.y);
		float2 normVec = normalize(vec);
		float2 normLightDir = normalize(lightDir);

		float dotProduct = dot(normVec, normLightDir);
		float i_r = 0.9f; // inner radius
		float o_r = 0.75; // outer radius
		if (dotProduct > i_r)
			color += m_color / radius;
		else if (dotProduct > o_r)
			color += lerp(m_color, float3(0.0f, 0.0f, 0.0f), (0.9f - dotProduct) / (i_r - o_r)) / radius;
	}

	// point lights
	if (m_type == 0)
	{
		if (radius == 0.0f)
			radius = 1.0f;
		color += m_color / radius;
	}
}

void Light::Update(const Camera& cam)
{
	m_screenPos = WorldToScreen(m_pos.x, m_pos.y, cam);
	m_intensity = cam.m_scale;
	//m_maxRadius *= cam.m_scale;
}

//
//// Point Lights
//PointLight::PointLight(float2 pos, float3 color) : Light(pos, color)
//{
//	m_pos.x = pos.x;
//	m_pos.y = pos.y;
//	m_color = color;
//
//	float max = fmaxf(color.x, fmaxf(color.y, color.z));
//	float colorPercent = 0.001f;
//	m_maxRadius = sqrtf(max / colorPercent) * 10.0f;
//}
//
//
//// Spot Lights
//SpotLight::SpotLight(float2 pos, float3 color) : Light(pos, color)
//{
//	m_pos.x = pos.x;
//	m_pos.y = pos.y;
//	m_color = color;
//
//	float max = fmaxf(color.x, fmaxf(color.y, color.z));
//	float colorPercent = 0.001f;
//	m_maxRadius = sqrtf(max / colorPercent) * 10.0f;
//}