#include "precomp.h"
#include "Entity.h"

Entity::Entity()
{
	m_pos = { 0.0f, 0.0f };
	m_direction = { 0.0f, 0.0f };
	m_temp_center = { 0.0f, 0.0f };
	m_speed = 0.1f;
	m_hp = 1;
	m_cooldown = 1;
	m_charging = 0;
	m_colorMultiplier = 1.0f;
	m_body = new Circle({ 0.0, 0.0f }, 1.0f, { 1.0f, 1.0f, 1.0f }, 0);
	m_image = new fSurface("assets/ball.png");
	bBoxColor = { 0.0f, 0.0f, 0.0f };
	m_aabbUpperLeft = { 0.0f, 0.0f };
	m_aabbLowerRight = { 0.0f, 0.0f };
	m_colorBoundBoxUpperLeft = { 0.0f, 0.0f };
	m_colorBoundBoxLowerRight = { 0.0f, 0.0f };
	m_colorBoundBoxHalfLength = 0.0f;
	m_colorBoundBoxUpperLeftScr = { 0.0f, 0.0f };
	m_colorBoundBoxLowerRightScr = { 0.0f, 0.0f };
	m_colorBoundBoxHalfLengthScr = 0.0f;
}
void Entity::Update(const float& deltaTime, const Camera& cam)
{
	m_pos += m_direction * deltaTime * m_speed;
	m_body->m_center = m_pos;
	m_aabbUpperLeft = m_pos - m_body->m_radius;
	m_aabbLowerRight = m_pos + m_body->m_radius;
	m_colorBoundBoxUpperLeft = m_pos - m_colorBoundBoxHalfLength;
	m_colorBoundBoxLowerRight = m_pos + m_colorBoundBoxHalfLength;
	m_colorBoundBoxUpperLeftScr = WorldToScreen(m_colorBoundBoxUpperLeft.x, m_colorBoundBoxUpperLeft.y, cam);
	m_colorBoundBoxLowerRightScr = WorldToScreen(m_colorBoundBoxLowerRight.x, m_colorBoundBoxLowerRight.y, cam);
	m_colorBoundBoxHalfLengthScr = m_colorBoundBoxHalfLength * cam.m_scale;
}

void Entity::Draw(fSurface* fscreen, const Camera& cam, const float& intensity)
{
	PlotTexture(m_pos, fscreen, cam, intensity);
	//float2 m_aabbUpperLeftScr = WorldToScreen(m_aabbUpperLeft.x, m_aabbUpperLeft.y, cam);
	//float2 m_aabbLowerRightScr = WorldToScreen(m_aabbLowerRight.x, m_aabbLowerRight.y, cam);

}

void Entity::PlotTexture(float2 imagePos, fSurface* fscreen, const Camera& cam, const float& intensity)
{
	imagePos = { imagePos.x - m_image->width / 2, imagePos.y - m_image->height / 2 };
	float2 imageUpperLeft = imagePos;
	float2 imageLowerRight = imagePos + m_image->width;
	float2 imageUpperRight = { imageLowerRight.x, imageUpperLeft.y };
	float2 imageLowerLeft = { imageUpperLeft.x, imageLowerRight.y };
	float2 imageUpperLeftScr = WorldToScreen(imageUpperLeft.x, imageUpperLeft.y, cam);
	float2 imageLowerRightScr = WorldToScreen(imageLowerRight.x, imageLowerRight.y, cam);
	float2 imageUpperRightScr = WorldToScreen(imageUpperRight.x, imageUpperRight.y, cam);
	float2 imageLowerLeftScr = WorldToScreen(imageLowerLeft.x, imageLowerLeft.y, cam);

	float2 aabbUpperLeftScr = { WORLDWIDTH, WORLDHEIGHT };
	//aabbUpperLeftScr.x = std::fminf(imageUpperLeftScr.x, std::fminf(imageLowerRightScr.x, std::fminf(imageUpperRightScr.x, imageLowerLeftScr.x)));
	aabbUpperLeftScr.x = std::min({ imageUpperLeftScr.x, imageLowerRightScr.x, imageUpperRightScr.x, imageLowerLeftScr.x });
	//aabbUpperLeftScr.y = std::fminf(imageUpperLeftScr.y, std::fminf(imageLowerRightScr.y, std::fminf(imageUpperRightScr.y, imageLowerLeftScr.y)));
	aabbUpperLeftScr.y = std::min({ imageUpperLeftScr.y, imageLowerRightScr.y, imageUpperRightScr.y, imageLowerLeftScr.y });
	float2 aabbLowerRightScr = { -WORLDWIDTH, -WORLDHEIGHT };
	//aabbLowerRightScr.x = std::fmaxf(imageUpperLeftScr.x, std::fmaxf(imageLowerRightScr.x, std::fmaxf(imageUpperRightScr.x, imageLowerLeftScr.x)));
	aabbLowerRightScr.x = std::max({ imageUpperLeftScr.x, imageLowerRightScr.x, imageUpperRightScr.x, imageLowerLeftScr.x });
	//aabbLowerRightScr.y = std::fmaxf(imageUpperLeftScr.y, std::fmaxf(imageLowerRightScr.y, std::fmaxf(imageUpperRightScr.y, imageLowerLeftScr.y)));
	aabbLowerRightScr.y = std::max({ imageUpperLeftScr.y, imageLowerRightScr.y, imageUpperRightScr.y, imageLowerLeftScr.y });

	//fscreen->fBox(aabbUpperLeftScr.x, aabbUpperLeftScr.y, aabbLowerRightScr.x, aabbLowerRightScr.y, { 1.0f, 0.0f, 0.0f });

	const int height = aabbLowerRightScr.y - aabbUpperLeftScr.y;
	const int width = aabbLowerRightScr.x - aabbUpperLeftScr.x;
	for (int y = 0; y < height; y++)
	{
		float2 A = ScreenToWorld(aabbUpperLeftScr.x, y + aabbUpperLeftScr.y, cam);
		float2 B = ScreenToWorld(aabbLowerRightScr.x, y + aabbUpperLeftScr.y, cam);
		float2 AtoB = B - A;
		float2 deltaAtoB = float2(AtoB.x, AtoB.y) / width;
		for (int x = 0; x < width; x++)
		{
			int imageCoordX = static_cast<int>(-imagePos.x + A.x + x * deltaAtoB.x);
			int imageCoordY = static_cast<int>(-imagePos.y + A.y + x * deltaAtoB.y);

			int screenCoordX = static_cast<int>(aabbUpperLeftScr.x) + x;
			int screenCoordY = static_cast<int>(aabbUpperLeftScr.y) + y;
			int screenIndex = static_cast<int>(aabbUpperLeftScr.x) + x + (static_cast<int>(aabbUpperLeftScr.y) + y) * SCRHEIGHT;
			// props to Daniel for helping me with the non-repeating texture
			if (imageCoordX >= 0 && imageCoordX < m_image->width && imageCoordY >= 0 && imageCoordY < m_image->height
				&& screenCoordX >= 0 && screenCoordX < SCRWIDTH && screenCoordY >= 0 && screenCoordY < SCRHEIGHT)
			{
				//fscreen->pixels[x + y * SCRWIDTH] = m_image->pixels[imageCoordX + m_image->width * imageCoordY];
				int index = (imageCoordX & (m_image->width - 1)) + (imageCoordY & (m_image->height - 1)) * m_image->width;
				if(m_image->pixels[index].x / m_color.x != 0.0f && m_image->pixels[index].y / m_color.y != 0.0f && m_image->pixels[index].z / m_color.z != 0.0f)
					fscreen->pixels[screenIndex] = m_image->pixels[index] * m_color * intensity * m_colorMultiplier;
			}
		}
	}
	m_colorMultiplier = 1.0f;
}

bool Entity::AABBCollision(const float2& upperLeftPoint, const float2& lowerRightPoint)
{
	if (m_aabbLowerRight.x >= upperLeftPoint.x && m_aabbUpperLeft.x <= lowerRightPoint.x
		&& m_aabbLowerRight.y >= upperLeftPoint.y && m_aabbUpperLeft.y <= lowerRightPoint.y)
	{
		return true;
	}
	return false;

	//if (m_aabbUpperLeft.x > lowerRightPoint.x || upperLeftPoint.x > m_aabbLowerRight.x)
	//{
	//	//bBoxColor = { 0.0f, 1.0f, 0.0f };
	//	return false;
	//}
	//if (m_aabbUpperLeft.y > lowerRightPoint.y || upperLeftPoint.y > m_aabbLowerRight.y)
	//{
	//	{
	//		//bBoxColor = { 0.0f, 1.0f, 0.0f };
	//		return false;
	//	}
	//}
	//return true;
}

bool Entity::Circle_Line_Intersect(const float2& v1, const float2& point)
{
	float2 v2 = { point.x - m_temp_center.x, point.y - m_temp_center.y };

	float a = dot(v1, v1);
	float b = 2 * dot(v2, v1);
	float c = dot(v2, v2) - m_body->m_radius * m_body->m_radius;

	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0)
		return false;
	else
	{
		discriminant = sqrt(discriminant);

		float t1 = (-b - discriminant) / (2 * a);
		float t2 = (-b + discriminant) / (2 * a);

		if (t1 >= 0.0f && t1 <= 1.0f)
		{
			return true;
		}
		if (t2 >= 0.0f && t2 <= 1.0f)
		{
			return true;
		}

		return false;
	}
}

bool Entity::PolygonIntersect(const Poly& poly, const float& deltaTime)
{
	bool flag = false;
	float2 temp_pos = m_pos + m_direction * deltaTime * m_speed;
	m_temp_center = temp_pos;
	m_aabbUpperLeft = temp_pos - m_body->m_radius;
	m_aabbLowerRight = temp_pos + m_body->m_radius;

	if (AABBCollision(poly.m_aabbUpperLeft, poly.m_aabbLowerRight))
	{
		for (int i = 1; i < poly.points.size(); i++)
		{
			float2 a = poly.points[i - 1].point;
			float2 b = poly.points[i].point;
			float2 AtoB = b - a;
			if (Circle_Line_Intersect(AtoB, a))
			{
				flag = true;
				break;
			}
		}
		float2 a = poly.points[poly.points.size() - 1].point;
		float2 b = poly.points[0].point;
		float2 AtoB = b - a;
		if (Circle_Line_Intersect(AtoB, a))
			flag = true;
	}
	else
		flag = false;
	return flag;
}

bool Entity::CircleIntersect(const Circle& circle)
{
	return false;
}

Player::Player(float2 pos, Circle& body, float speed) : Entity()
{
	m_image = new fSurface("assets/wizard.png");
	for (int i = 0; i < m_image->width * m_image->height; i++)
	{
		m_image->pixels[i] *= 5.0f;
	}

	m_pos = pos;
	m_body = &body;
	m_speed = speed;
	m_hp = 25;
	m_maxHp = m_hp;
	m_cooldown = 5;

	m_colorBoundBoxHalfLength = m_body->m_radius / sqrtf(2);
	m_colorBoundBoxUpperLeft = m_pos - m_colorBoundBoxHalfLength;
	m_colorBoundBoxLowerRight = m_pos + m_colorBoundBoxHalfLength;
}

bool CompareFloat(float a, float b, float epsilon)
{
	if (fabs(a - b) < epsilon)
		return true;
	else
		return false;
}

float CutFloat(float num)
{
	int integer_part = static_cast<int>(num);
	int decimal_part = static_cast<int>((num - integer_part) * 1000);
	float truncated_num = static_cast<float>(integer_part) + static_cast<float>(decimal_part) / 1000.0f;
	return truncated_num;
}

void Player::CalculateColor(fSurface* fscreen)
{
	float m_colorBoundBoxLengthScr = m_colorBoundBoxHalfLengthScr * 2;
	float3 colorSum = 0;
	//colorSum = fscreen->pixels[static_cast<int>(m_body->m_screenCenter.x) + static_cast<int>(m_body->m_screenCenter.y) * SCRWIDTH];
	for (int y = 0; y < m_colorBoundBoxLengthScr; y++)
	{
		for (int x = 0; x < m_colorBoundBoxLengthScr; x++)
		{
			int index = static_cast<int>(m_colorBoundBoxUpperLeftScr.x) + x + (static_cast<int>(m_colorBoundBoxUpperLeftScr.y) + y) * SCRWIDTH;
			if(index >= 0 && index < fscreen->width * fscreen->height)
				colorSum += fscreen->pixels[index];
		}
	}
	//float maxColor = GetMaxRGB(colorSum.x, colorSum.y, colorSum.z);
	float maxColor = std::fmaxf(colorSum.x, std::fmaxf(colorSum.y, colorSum.z));
	colorSum = { CutFloat(colorSum.x), CutFloat(colorSum.y), CutFloat(colorSum.z) };
	maxColor = CutFloat(maxColor);
	if (maxColor == colorSum.x && maxColor == colorSum.y && maxColor == colorSum.z)
	{
		m_body->m_color = { 5.0f, 5.0f, 5.0f };
		m_weaponColor = { 5.0f, 5.0f, 5.0f };
	}
	else if (maxColor == colorSum.x)
	{
		m_body->m_color = { 5.0f, 0.0f, 0.0f };
		m_weaponColor = { 5.0f, 0.0f, 0.0f };
	}
	else if (maxColor == colorSum.y)
	{
		m_body->m_color = { 0.0f, 5.0f, 0.0f };
		m_weaponColor = { 0.0f, 5.0f, 0.0f };
	}
	else
	{
		m_body->m_color = { 0.0f, 0.0f, 5.0f };
		m_weaponColor = { 0.0f, 0.0f, 5.0f };
	}
	//m_body->m_color = std::max(colorSum.x, std::max(colorSum.y, colorSum.z)) / (m_colorBoundBoxLength * m_colorBoundBoxLength);
}

Enemy::Enemy(float2 pos, Circle& body, float3 color, float speed)
{
	m_image = new fSurface("assets/ghost_transparent.png");
	for (int i = 0; i < m_image->width * m_image->height; i++)
	{
		//m_image->pixels[i] *= color;
	}

	m_pos = pos;
	m_body = &body;
	m_speed = speed;
	m_color = color;
	m_hp = 6;
	m_cooldown = 50;
}

void Enemy::DetectPlayer(const float2& playerPos)
{
	float sqrdEnemyPlayerDistance = sqrLength(playerPos - m_pos);
	if (sqrdEnemyPlayerDistance < m_sqrdDetectionDistance)
	{
		m_direction = normalize(playerPos - m_pos);
	}
	else
	{
		m_direction = { 0.0f, 0.0f };
	}
}

void Enemy::Attack(Player& player)
{
	m_charging++;
	if (m_charging > m_cooldown)
	{
		if (AABBCollision(player.m_aabbUpperLeft, player.m_aabbLowerRight))
		{
			m_charging = 0;
			player.m_colorMultiplier = 5.0f;
			player.m_hp--;
		}
	}
}


Projectile::Projectile(float2 pos, Circle& body, float3 color, float speed)
{
	m_image = new fSurface("assets/projectile.png");
	for (int i = 0; i < m_image->width * m_image->height; i++)
	{
		//m_image->pixels[i] *= color;
	}

	m_pos = pos;
	m_body = &body;
	m_speed = speed;
	m_color = color;
	m_cooldown = 50;
}


void Projectile::Attack(Enemy& enemy)
{
	if (AABBCollision(enemy.m_aabbUpperLeft, enemy.m_aabbLowerRight)
		&& CompareFloat(m_color.x, enemy.m_color.x, 0.001f)
		&& CompareFloat(m_color.y, enemy.m_color.y, 0.001f)
		&& CompareFloat(m_color.z, enemy.m_color.z, 0.001f))
	{
		m_lifetime = 0;
		enemy.m_colorMultiplier = 5.0f;
		enemy.m_hp--;
	}
}