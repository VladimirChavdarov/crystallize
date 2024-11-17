#include "precomp.h"
#include "Primitive.h"

// LINE
//Line::Line(float2 sp, float2 ep, float3 color, int id)
//{
//	m_id = id;
//	m_sp = sp;
//	m_ep = ep;
//	m_color = color;
//	m_originalColor = color;
//}
//
//void Line::Draw(fSurface* fscreen, const Camera& cam)
//{
//	float2 a = WorldToScreen(m_sp.x, m_sp.y, cam);
//	float2 b = WorldToScreen(m_ep.x, m_ep.y, cam);
//	fscreen->fLine(a.x, a.y, b.x, b.y, m_color);
//}


// CIRCLE 
Circle::Circle(float2 center, float radius, float3 color, int id)
{
	m_id = id;
	m_center = center;
	m_radius = radius;
	m_color = color;
	m_originalColor = color;
}

// using Bresenham's Circle Drawing Algorithm: https://iq.opengenus.org/bresenhams-circle-drawing-algorithm/
void Circle::Draw(fSurface* fscreen, const Camera& cam)
{
	float2 float_center = WorldToScreen(m_center.x, m_center.y, cam);
	int2 int_center = { static_cast<int>(float_center.x), static_cast<int>(float_center.y) };
	int int_radius = static_cast<int>(m_radius * cam.m_scale);
	int x = 0, y = int_radius; // x and y on a coordinate system with (0, 0) being the center of the circle
	int decesionParameter = 3 - 2 * static_cast<int>(m_radius);

	fscreen->fPlot(int_center.x + x, int_center.y + y, m_color);
	fscreen->fPlot(int_center.x - x, int_center.y + y, m_color);
	fscreen->fPlot(int_center.x + x, int_center.y - y, m_color);
	fscreen->fPlot(int_center.x - x, int_center.y - y, m_color);
	fscreen->fPlot(int_center.x + y, int_center.y + x, m_color);
	fscreen->fPlot(int_center.x - y, int_center.y + x, m_color);
	fscreen->fPlot(int_center.x + y, int_center.y - x, m_color);
	fscreen->fPlot(int_center.x - y, int_center.y - x, m_color);

	while (y >= x)
	{
		x++;
		if (decesionParameter > 0)
		{
			y--;
			decesionParameter = decesionParameter + 4 * (x - y) + 10;
		}
		else
		{
			decesionParameter = decesionParameter + 4 * x + 6;
		}

		fscreen->fPlot(int_center.x + x, int_center.y + y, m_color);
		fscreen->fPlot(int_center.x - x, int_center.y + y, m_color);
		fscreen->fPlot(int_center.x + x, int_center.y - y, m_color);
		fscreen->fPlot(int_center.x - x, int_center.y - y, m_color);
		fscreen->fPlot(int_center.x + y, int_center.y + x, m_color);
		fscreen->fPlot(int_center.x - y, int_center.y + x, m_color);
		fscreen->fPlot(int_center.x + y, int_center.y - x, m_color);
		fscreen->fPlot(int_center.x - y, int_center.y - x, m_color);
	}
}

void Circle::Draw_3dgep(fSurface* fscreen)
{
	for (int i = 0; i < 64; i++)
	{
		const float r1 = static_cast<float>(i) * PI / 32;
		const float r2 = static_cast<float>(i + 1) * PI / 32;

		fscreen->fLine(m_center.x - m_radius * sinf(r1), m_center.y - m_radius * cosf(r1),
			m_center.x - m_radius * sinf(r2), m_center.y - m_radius * cosf(r2), m_color);
	}
}

void Circle::DrawAABB(fSurface* fscreen)
{
	//float2 aabbUpperLeftScr = m_screenCenter - m_screenRadius;
	//float2 aabbLowerRightScr = m_screenCenter + m_screenRadius;

	fscreen->fBox(static_cast<int>(m_aabbUpperLeftScr.x), static_cast<int>(m_aabbUpperLeftScr.y),
		static_cast<int>(m_aabbLowerRightScr.x), static_cast<int>(m_aabbLowerRightScr.y), { 1.0f, 0.0f, 0.0f });
}

// TODO: make aabb checkk to see if the circle is completely offscreen. If yes, don't draw call fPlot
// UPDATE: Turns out this kind of rendering will be depricated in a few weeks so I will put this on hold and maybe delete it later.
//D    C
//  ___
// |   |
// |   |
//  ___
//A     B
bool Circle::isFullyOffScreen()
{
	return true;
}

void Circle::SelectPrimitive(const int2& mousePos, const Camera& cam)
{
	isSelected = false;
	m_color = m_originalColor;
	float2 mousePosWorld = ScreenToWorld(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y), cam);
	float2 distanceVector = { mousePosWorld.x - m_center.x, mousePosWorld.y - m_center.y };
	if (lengthsqrd(distanceVector) < m_radius * m_radius)
	{
		isSelected = true;
		m_color = { 100.0f, 100.0f, 100.0f };
	}
}

void Circle::Update(const float2& offset)
{
	m_center += offset;
}

void Circle::UpdateScreenPos(const Camera& cam)
{
	m_screenCenter = WorldToScreen(m_center.x, m_center.y, cam);
	m_screenRadius = m_radius * cam.m_scale;

	m_aabbUpperLeftScr = m_screenCenter - m_screenRadius;
	m_aabbLowerRightScr = m_screenCenter + m_screenRadius;
}

bool Circle::AABBCheck(const float2& rayStart, const float2& rayEnd)
{
	float2 l_rayStart = float2(std::min(rayStart.x, rayEnd.x), std::min(rayStart.y, rayEnd.y));
	float2 l_rayEnd = float2(std::max(rayStart.x, rayEnd.x), std::max(rayStart.y, rayEnd.y));

	if (m_aabbUpperLeftScr.x > l_rayEnd.x || l_rayStart.x > m_aabbLowerRightScr.x)
	{
		//bBoxColor = { 0.0f, 1.0f, 0.0f };
		return false;
	}
	if (m_aabbUpperLeftScr.y > l_rayEnd.y || l_rayStart.y > m_aabbLowerRightScr.y)
	{
		{
			//bBoxColor = { 0.0f, 1.0f, 0.0f };
			return false;
		}
	}
	return true;
}

// reference: https://stackoverflow.com/questions/1073336/circle-line-segment-collision-detection-algorithm
bool Circle::Intersect(const float2& v1, const float2& lightCoord)
{
	float2 v2 = { lightCoord.x - m_screenCenter.x, lightCoord.y - m_screenCenter.y };

	float a = dot(v1, v1);
	float b = 2 * dot(v2, v1);
	float c = dot(v2, v2) - m_screenRadius * m_screenRadius;

	float discriminant = b*b - 4*a*c;

	if (discriminant < 0)
		return false;
	else
	{
		discriminant = sqrt(discriminant);

		float t1 = (-b - discriminant) / (2 * a);
		float t2 = (-b + discriminant) / (2 * a);

		if (t1 >= 0.0f && t1 <= 1.0f)
		{
			if (t2 > 1.0f)
				return false;
			return true;
		}
		if (t2 >= 0.0f && t2 <= 1.0f)
		{
			return true;
		}

		return false;
	}
}

// POLYGON
inline int randomNum(int min, int max)
{
	return rand() % (max - min + 1) + min;
}
Poly::Poly(float2 center, float radius, float3 color, int id, int numSides, vector<float>& angles, bool randomized_radius)
{
	m_id = id;
	m_color = color;
	m_originalColor = color;
	m_center = center;
	m_radius = radius;
	m_numSides = numSides;
	for (int i = 0; i < angles.size(); i++)
	{
		float2 aPoint = { 0.0f, 0.0f };
		if(randomized_radius)
			aPoint = { m_center.x + cosf(DEG_TO_RAD(angles[i])) * randomNum(static_cast<int>(m_radius - m_radius / 5), static_cast<int>(m_radius + m_radius / 5)), m_center.y + sinf(DEG_TO_RAD(angles[i])) * randomNum(static_cast<int>(m_radius - 30), static_cast<int>(m_radius + 30)) };
		else
			aPoint = { m_center.x + cosf(DEG_TO_RAD(angles[i])) * m_radius, m_center.y + sinf(DEG_TO_RAD(angles[i])) * m_radius };

		if (aPoint.x < m_aabbUpperLeft.x)
			m_aabbUpperLeft.x = aPoint.x;
		if (aPoint.y < m_aabbUpperLeft.y)
			m_aabbUpperLeft.y = aPoint.y;
		if (aPoint.x > m_aabbLowerRight.x)
			m_aabbLowerRight.x = aPoint.x;
		if (aPoint.y > m_aabbLowerRight.y)
			m_aabbLowerRight.y = aPoint.y;
		points.push_back({ aPoint, {0.0f, 0.0f}, DEG_TO_RAD(angles[i]) });
	}
}

void Poly::Draw(fSurface* fscreen, const Camera& cam)
{
	float2 lastA = { 0.0f, 0.0f };
	for (int i = 0; i < points.size() - 1; i++)
	{
		float2 a = WorldToScreen(points[i].point.x, points[i].point.y, cam);
		float2 b = WorldToScreen(points[i + 1].point.x, points[i + 1].point.y, cam);
		lastA = b;
		fscreen->fLine(a.x, a.y, b.x, b.y, m_color);
	}
	float2 lastB = WorldToScreen(points[0].point.x, points[0].point.y, cam);
	fscreen->fLine(lastA.x, lastA.y, lastB.x, lastB.y, m_color);
}

void Poly::DrawAABB(fSurface* fscreen)
{
	fscreen->fBox(static_cast<int>(m_aabbUpperLeftScr.x), static_cast<int>(m_aabbUpperLeftScr.y),
		static_cast<int>(m_aabbLowerRightScr.x), static_cast<int>(m_aabbLowerRightScr.y), bBoxColor);
}

void Poly:: SelectPrimitive(const int2& mousePos, const Camera& cam)
{
	
}

void Poly::Update(const float2& offset)
{
	m_center += offset;
	for (int i = 0; i < points.size(); i++)
	{
		points[i].point += offset;
	}
}

void Poly::UpdateScreenPos(const Camera& cam)
{
	m_aabbUpperLeftScr = { WORLDWIDTH / 2, WORLDHEIGHT / 2 };
	m_aabbLowerRightScr = { -WORLDWIDTH / 2, -WORLDHEIGHT / 2 };
	for (int i = 0; i < points.size(); i++)
	{
		points[i].scrPoint = WorldToScreen(points[i].point.x, points[i].point.y, cam);
		if (points[i].scrPoint.x < m_aabbUpperLeftScr.x)
			m_aabbUpperLeftScr.x = points[i].scrPoint.x;
		if (points[i].scrPoint.y < m_aabbUpperLeftScr.y)
			m_aabbUpperLeftScr.y = points[i].scrPoint.y;
		if (points[i].scrPoint.x > m_aabbLowerRightScr.x)
			m_aabbLowerRightScr.x = points[i].scrPoint.x;
		if (points[i].scrPoint.y > m_aabbLowerRightScr.y)
			m_aabbLowerRightScr.y = points[i].scrPoint.y;
	}
}

bool Poly::AABBCheck(const float2& rayStart, const float2& rayEnd)
{
	//float2 l_rayStart = rayStart;
	//float2 l_rayEnd = rayEnd;
	float2 l_rayStart = float2(std::min(rayStart.x, rayEnd.x), std::min(rayStart.y, rayEnd.y));
	float2 l_rayEnd = float2(std::max(rayStart.x, rayEnd.x), std::max(rayStart.y, rayEnd.y));

	if (m_aabbUpperLeftScr.x > l_rayEnd.x || l_rayStart.x > m_aabbLowerRightScr.x)
	{
		//bBoxColor = { 0.0f, 1.0f, 0.0f };
		return false;
	}
	if (m_aabbUpperLeftScr.y > l_rayEnd.y || l_rayStart.y > m_aabbLowerRightScr.y)
	{
		{
			//bBoxColor = { 0.0f, 1.0f, 0.0f };
			return false;
		}
	}
	return true;
}

inline bool Poly::Intersect(const float2& v1, const float2 &lightCoord)
{
	//bool intersection = false;
	for (int i = 1; i < points.size(); i++)
	{
		float2 a = points[i-1].scrPoint;
		float2 b = points[i].scrPoint;
		float2 v2 = b - a;
		float denom = v1.x * v2.y - v1.y * v2.x;
		float inv_denom = 1 / denom;

		float2 v3 = lightCoord - a;
		float uaNum = v2.x * v3.y - v2.y * v3.x;
		float ubNum = v1.x * v3.y - v1.y * v3.x;
		float ua = uaNum * inv_denom;
		float ub = ubNum * inv_denom;
		if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
		{
			// Calculate the intersection point
			pointOfIntersection.x = a.x + ua * (b.x - a.x);
			pointOfIntersection.y = b.y + ua * (b.y - a.y);
			//intersection = true;
			return true;
			break;
		}
		else
		{
			// The intersection point is outside one or both line segments
			//intersection = false;
			//return false;
		}
	}
	if (true/*!intersection*/)
	{
		float2 a = points[points.size() - 1].scrPoint;
		float2 v2 = points[0].scrPoint - a;
		float denom = v1.x * v2.y - v1.y * v2.x;
		if (denom == 0)
			return false;

		float2 v3 = lightCoord - a;
		float uaNum = v2.x * v3.y - v2.y * v3.x;
		float ubNum = v1.x * v3.y - v1.y * v3.x;
		float ua = uaNum / denom;
		float ub = ubNum / denom;
		if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
		{
			// Calculate the intersection point
			pointOfIntersection.x = a.x + ua * (points[0].scrPoint.x - a.x);
			pointOfIntersection.y = points[0].scrPoint.y + ua * (points[0].scrPoint.y - a.y);
			return true;
			//intersection = true;
		}
		else
		{
			// The intersection point is outside one or both line segments
			//return false;
			//intersection = false;
		}
	}
	
	return false;
	//return intersection;
}

// with help from Jeremiah
// mirroring a point
// points A and B: start and end point of the mirror
// point P: initial point
// point Q: mirrored point
// point C: closest point to P on the mirror (vector P-C is perpendicular to A-B)
//            A
//  Q       /
//   o   C /    
//        o 
//       /    o
//      /      P
//     B       
float2 MirrorPoint(const float2& point, const float2& mirrorPointA, const float2& mirrorPointB)
{
	float2 AB = mirrorPointB - mirrorPointA;
	float t = dot(point - mirrorPointA, AB) / dot(AB, AB);
	float2 closestPointOnMirror =  mirrorPointA + t * AB; // C (see picture above)
	float2 vec = closestPointOnMirror - point; // vector from P to C (see picture above)

	return point + 2 * vec;
}

// make this function return something and then make all of the light calculations in DrawLights in game.cpp. For example return the vector fromt he mirrored light to pixel or sth
bool Poly::MirrorIntersect(const float2& pixelCoord, const Light& light, const vector<Poly*>& polygons, float3& color)
{
	for (int i = 1; i < points.size(); i++)
	{
		float2 a = points[i - 1].scrPoint;
		float2 b = points[i].scrPoint;
		float2 mirrorLightCoord = MirrorPoint(light.m_screenPos, a, b);
		//float2 mirrorLightCoord = lightCoord;
		float2 v1 = pixelCoord - mirrorLightCoord;
		float2 v2 = b - a;
		float denom = v1.x * v2.y - v1.y * v2.x;
		float inv_denom = 1 / denom;

		float2 v3 = mirrorLightCoord - a;
		float uaNum = v2.x * v3.y - v2.y * v3.x;
		float ubNum = v1.x * v3.y - v1.y * v3.x;
		float ua = uaNum * inv_denom;
		float ub = ubNum * inv_denom;
		if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
		{
			// Calculate the intersection point
			pointOfIntersection.x = a.x + ua * (b.x - a.x);
			pointOfIntersection.y = b.y + ua * (b.y - a.y);

			bool next = false;
			for (int j = 0; j < polygons.size(); j++)
			{
				// if there's no AABB collision this polygon is not relevant
				if (!polygons[j]->AABBCheck(light.m_screenPos, pixelCoord)) continue;

				if (polygons[j]->Intersect(pixelCoord - pointOfIntersection, pointOfIntersection))
				{
					next = true;
					break;
				}
			}
			if (next) return false;

			float inv_radius = 1 / (length(v1) / light.m_intensity);

			//spotlights
			if (light.m_type == SPOT_LIGHT)
			{

				float2 lightDir = float2(10 - mirrorLightCoord.x, 10 - mirrorLightCoord.y);
				float2 normLightDir = normalize(lightDir);
				float2 normVec = normalize(v1);
				float dotProduct = dot(normVec, normLightDir);
				// smooth-edge
				float i_r = 0.9f; // inner radius
				float o_r = 0.75; // outer radius
				if (dotProduct > i_r)
					color += light.m_intensity * inv_radius;
				else if (dotProduct > o_r)
					color += lerp(light.m_intensity, float3(0.0f, 0.0f, 0.0f), (0.9f - dotProduct) / (i_r - o_r)) * inv_radius;
			}


			//point lights
			if (light.m_type == POINT_LIGHT)
			{
				color += light.m_color * inv_radius;
			}


			return true;
			break;
		}
	}

	return false;
}