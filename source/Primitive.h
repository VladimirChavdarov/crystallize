#pragma once
#include "Camera.h"
#include "Light.h"

class Primitive
{
public:
	Primitive(){}
	virtual bool Intersect(const float2& v1, const float2 &lightCoord) = 0;
	virtual void Draw(fSurface* fscreen, const Camera& cam) = 0;
	virtual void SelectPrimitive(const int2& mousePos, const Camera& cam) = 0;

	bool isSelected = false;
	float3 m_color = { 0.0f, 0.0f, 0.0f };
	float3 m_originalColor = { 0.0f, 0.0f, 0.0f };
	int m_id = 0;

protected:
};

class Circle : public Primitive
{
public:
	Circle(float2 center, float radius, float3 color, int id);
	void Draw(fSurface* fscreen, const Camera& cam) override;
	void DrawAABB(fSurface* fscreen);
	bool isFullyOffScreen();
	void Draw_3dgep(fSurface* fscreen);
	void SelectPrimitive(const int2& mousePos, const Camera& cam) override;
	void Update(const float2& offset);
	void UpdateScreenPos(const Camera& cam);
	bool AABBCheck(const float2& rayStart, const float2& rayEnd);
	bool Intersect(const float2& v1, const float2 &lightCoord) override;

	float2 m_center;
	float2 m_screenCenter = { 0.0f, 0.0f };
	float m_radius;
	float m_screenRadius = 0.0f;
	float2 m_aabbUpperLeft = { WORLDWIDTH / 2, WORLDHEIGHT / 2 }, m_aabbLowerRight = { -WORLDWIDTH / 2, -WORLDHEIGHT / 2 };
	float2 m_aabbUpperLeftScr = { WORLDWIDTH / 2, WORLDHEIGHT / 2 }, m_aabbLowerRightScr = { -WORLDWIDTH / 2, -WORLDHEIGHT / 2 };
private:
};

struct point_and_angle
{
	float2 point;
	float2 scrPoint;
	float radAngle;
};

class Poly : public Primitive
{
public:
	Poly(float2 center, float radius, float3 color, int id, int numSides, vector<float>& angles, bool randomized_radius);
	void Draw(fSurface* fscreen, const Camera& cam) override;
	void DrawAABB(fSurface* fscreen);
	void SelectPrimitive(const int2& mousePos, const Camera& cam) override;
	void Update(const float2& offset);
	void UpdateScreenPos(const Camera& cam);
	bool Intersect(const float2& v1, const float2 &lightCoord) override;
	bool MirrorIntersect(const float2& pixelCoord, const Light& light, const vector<Poly*>& polygons, float3& color);
	bool AABBCheck(const float2& rayStart, const float2& rayEnd);

	vector<point_and_angle> points;
	int m_numSides;
	float2 m_center;
	float m_radius;
	float2 m_aabbUpperLeft = {WORLDWIDTH / 2, WORLDHEIGHT / 2}, m_aabbLowerRight = { -WORLDWIDTH / 2, -WORLDHEIGHT / 2 };
	float2 m_aabbUpperLeftScr = { WORLDWIDTH / 2, WORLDHEIGHT / 2 }, m_aabbLowerRightScr = { -WORLDWIDTH / 2, -WORLDHEIGHT / 2 };
	float3 bBoxColor = { 0.0f, 1.0f, 0.0f };

	float2 pointOfIntersection = { 0.0f, 0.0f };
};