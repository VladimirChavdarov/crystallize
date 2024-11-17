#pragma once
#include "Primitive.h"
#include "Camera.h"

class Entity
{
public:
	Entity();

	void Update(const float& deltaTime, const Camera& cam);
	void Draw(fSurface* fscreen, const Camera& cam, const float& intensity = 1.0f);
	void PlotTexture(float2 imagePos, fSurface* fscreen, const Camera& cam, const float& intensity = 1.0f);
	bool AABBCollision(const float2& upperLeftPoint, const float2& lowerRightPoint);
	bool Circle_Line_Intersect(const float2& v1, const float2& point);
	bool PolygonIntersect(const Poly& poly, const float& deltaTime);
	bool CircleIntersect(const Circle& circle);

	fSurface* m_image;
	Circle* m_body;

	float3 m_color = { 1.0f, 1.0f, 1.0f };
	float2 m_pos;
	float2 m_direction = { 0.0f, 0.0f };
	float2 m_temp_center = { 0.0f, 0.0f };
	float m_speed = 0.1f;
	int m_hp = 1;
	int m_cooldown = 1;
	int m_charging = 0;
	float m_colorMultiplier = 1.0f;

	float3 bBoxColor = { 0.0f, 0.0f, 0.0f };
	float2 m_aabbUpperLeft = { 0.0f, 0.0f };
	float2 m_aabbLowerRight = { 0.0f, 0.0f };
	float2 m_colorBoundBoxUpperLeft = { 0.0f, 0.0f };
	float2 m_colorBoundBoxLowerRight = { 0.0f, 0.0f };
	float m_colorBoundBoxHalfLength = 0.0f;
	float2 m_colorBoundBoxUpperLeftScr = { 0.0f, 0.0f };
	float2 m_colorBoundBoxLowerRightScr = { 0.0f, 0.0f };
	float m_colorBoundBoxHalfLengthScr = 0.0f;
};

class Player : public Entity
{
public:
	Player(float2 pos, Circle& body, float speed);

	void CalculateColor(fSurface* fscreen);

	//float3 m_color = { 1.0f, 1.0f, 1.0f };
	float3 m_weaponColor = { 1.0f, 1.0f, 1.0f };
	int m_maxHp;
};

class Enemy : public Entity
{
public:
	Enemy(float2 pos, Circle& body, float3 color, float speed);

	void DetectPlayer(const float2& playerPos);
	void Attack(Player& player);

	//float3 m_color;
	float detectionDistance = 700.0f;
	float m_sqrdDetectionDistance = detectionDistance * detectionDistance;
};

class Projectile : public Entity
{
public:
	Projectile(float2 pos, Circle& body, float3 color, float speed);

	void Attack(Enemy& enemy);

	int m_lifetime = 100;
	//float3 m_color;
};