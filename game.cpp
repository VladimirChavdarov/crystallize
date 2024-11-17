// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/UU - Jacco Bikker - 2006-2023

#include "precomp.h"
#include "game.h"

void Game::KeyDown(int key)
{
	switch (key)
	{
	case GLFW_KEY_A:
	{
		player->m_direction.x = -1.0f;
		break;
	}
	case GLFW_KEY_D:
	{
		player->m_direction.x = 1.0f;
		break;
	}
	case GLFW_KEY_W:
	{
		player->m_direction.y = -1.0f;
		break;
	}
	case GLFW_KEY_S:
	{
		player->m_direction.y = 1.0f;
		break;
	}
	case GLFW_KEY_TAB:
	{
		if (game_state == GAME)
		{
			if (mReleased)
				playerTempPos = player->m_pos;
			player->m_pos = { -4096.0f, -4096.0f };
			cam.m_scale = 0.110;
			showMap = true;
		}
		break;
	}
	case GLFW_KEY_Z:
	{
		scale_modifier = 1.01f;
		break;
	}
	case GLFW_KEY_X:
	{
		scale_modifier = 0.99f;
		break;
	}
	case GLFW_KEY_LEFT:
	{
		rotation_modifier = 2.0f;
		break;
	}
	case GLFW_KEY_RIGHT:
	{
		rotation_modifier = -2.0f;
		break;
	}
	case GLFW_KEY_BACKSPACE:
	{
		cam.Reset();
		break;
	}
	case GLFW_KEY_ENTER:
	{
		game_state = GAME;
		InitGame(m_deltaTime);
		break;

	}
	case GLFW_KEY_L:
	{
		game_state = STRESS_TEST;
		InitStressTest();
		break;
	}
	case GLFW_KEY_H:
	{
		game_state = HOW_TO_PLAY;
		break;
	}
	case GLFW_KEY_B:
	{
		game_state = MAIN_MENU;
		break;
	}
	case GLFW_KEY_Y:
	{
		game_state = END_SCREEN;
		InitEndScreen();
		break;
	}
	default:
		break;
	}
}

void Game::KeyUp(int key)
{
	switch (key)
	{
	case GLFW_KEY_A:
	{
		player->m_direction.x = 0.0f;
		break;
	}
	case GLFW_KEY_D:
	{
		player->m_direction.x = 0.0f;
		break;
	}
	case GLFW_KEY_W:
	{
		player->m_direction.y = 0.0f;
		break;
	}
	case GLFW_KEY_S:
	{
		player->m_direction.y = 0.0f;
		break;
	}
	case GLFW_KEY_TAB:
	{
		if (game_state == GAME)
		{
			cam.m_scale = 1.0f;
			player->m_pos = playerTempPos;
			mReleased = true;
			showMap = false;
		}
		break;
	}
	case GLFW_KEY_Z:
	{
		scale_modifier = 1.000f;
		break;
	}
	case GLFW_KEY_X:
	{
		scale_modifier = 1.000f;
		break;
	}
	case GLFW_KEY_LEFT:
	{
		rotation_modifier = 0.0f;
		break;
	}
	case GLFW_KEY_RIGHT:
	{
		rotation_modifier = 0.0f;
		break;
	}
	default:
		break;
	}
}

void Game::MouseDown(int button)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_1:
	{
		mouseLeft = true;
		break;
	}
	case GLFW_MOUSE_BUTTON_2:
	{
		mouseRight = true;
		break;
	}
	case GLFW_MOUSE_BUTTON_4:
	{
		scale_modifier = 1 + 0.01f;
		break;
	}
	case GLFW_MOUSE_BUTTON_5:
	{
		scale_modifier = 1 - 0.01f;
		break;
	}
	}
}

void Game::MouseUp(int button)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_1:
	{
		mouseLeft = false;
		break;
	}
	case GLFW_MOUSE_BUTTON_2:
	{
		mouseRight = true;
		break;
	}
	case GLFW_MOUSE_BUTTON_4:
	{
		scale_modifier = 1.000f;
		break;
	}
	case GLFW_MOUSE_BUTTON_5:
	{
		scale_modifier = 1.000f;
		break;
	}
	}
}

void Game::MouseWheel(float y)
{
	if (y > 0.0f)
		wheel = 1.000f + y / 10.0f;
	else if (y < 0.0f)
		wheel = 1.000f + y / 10.0f;
}

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
inline int randomNum(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

void Game::Init()
{
	// anything that happens only once at application start goes here
	srand(static_cast<unsigned int>(time(0)));
	InitMainMenu();
}








// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// ----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	// clear the screen to black
	m_deltaTime = deltaTime;
	// If I clear the screen to white, I omit an if condition when lighting up pixels that are not affected by backdrop (hint from Boyko)
	fscreen->fClear({ 1.0f, 1.0f, 1.0f });
	//fscreen->fClear({ 0.0f, 0.0f, 0.0f });

	if (game_state == MAIN_MENU || game_state == HOW_TO_PLAY)
	{
		MainMenu(deltaTime);
	}
	else if (game_state == GAME)
	{
		MainGame(deltaTime);
	}
	else if (game_state == STRESS_TEST)
	{
		StressTest(deltaTime);
	}
	else if (game_state == END_SCREEN)
	{
		EndScreen(deltaTime);
	}

	FPSCounter(deltaTime);
}


// ----------- Helper functions -----------

void Game::InitMainMenu()
{
	lightsToGoThrough = 1;
	playerBody = new Circle({ 0.0f, 0.0f }, 64.0f, { 10.0f, 10.0f, 10.0f }, 0);
	player = new Player({ 0.0f, 0.0f }, *playerBody, 0.5f);

	// create enemies
	for (int i = 0; i < 3; i++)
	{
		int colorType = i;
		float3 color;
		if (colorType == 0)
			color = { 2.0f, 0.0f, 0.0f };
		else if (colorType == 1)
			color = { 0.0f, 2.0f, 0.0f };
		else
			color = { 0.0f, 0.0f, 2.0f };
		float2 pos = {-74.0f + (64+10) * i, -100.0f};
		Circle* aBody = new Circle(pos, 32.0f, { 10.0f, 10.0f, 10.0f }, i+1);
		Enemy* aEnemy = new Enemy(pos, *aBody, color, 0.3f);
		enemies.push_back(aEnemy);
	}


	// create polygons
	float radius = 150.0f;
	vector<float> angles;
	int numSides = 4;
	for (int i = 0; i < 3; i++)
	{
		//numSides = randomNum(3, 13);
		//angles.push_back(0.0f);
		angles.push_back(static_cast<float>(randomNum(0, 360 / numSides)));
		for (int j = 1; j < numSides; j++)
		{
			angles.push_back(angles[j - 1] + 360 / numSides);
			//angles.push_back(randomNum(angles[j-1], 360));
		}
		Poly* aPoly = new Poly({ static_cast<float>(randomNum(static_cast<int>(- SCRWIDTH / 2 + radius / 2), static_cast<int>(0 - radius / 2))), static_cast<float>(randomNum(static_cast<int>(-SCRHEIGHT / 2 + radius / 2), static_cast<int>(0 - radius / 2)))}, radius, {static_cast<float>(randomNum(0, 10)) / 10.0f, static_cast<float>(randomNum(0, 10)) / 10.0f, static_cast<float>(randomNum(0, 10)) / 10.0f}, i + 1, numSides, angles, true);
		polygons.push_back(aPoly);
		angles.clear();
	}
	addLightSource({ 0.0f, 0.0f }, { 10.0f, 5.0f, 15.0f }, 1, 0);
	addLightSource(ScreenToWorld(50 + 0 * 308, 800, cam), { 15.0f, 5.0f, 7.0f }, 0, 1);
	addLightSource(ScreenToWorld(50 + 1 * 308, 800, cam), { 5.0f, 15.0f, 7.0f }, 0, 2);
	addLightSource(ScreenToWorld(50 + 2 * 308, 800, cam), { 7.0f, 5.0f, 15.0f }, 0, 3);

	
}

float offset = 0;
void Game::MainMenu(float& deltaTime)
{
	rotation_modifier = 0.02f * deltaTime;
	UpdateCameraMainMenu(deltaTime);

	float3 textColor = { 10.0f, 10.0f, 10.0f };
	if (game_state == MAIN_MENU)
	{
		lightsToGoThrough = 1;
		string title = "Crystallize";
		string mainGame = "Press Enter to start game";
		string limitTest = "Press L to start limit test";
		string howToPlay = "Press H to see how to play";
		string endScreen = "Press Y to skip to the end screen";
		fscreen->fCenterPrint(title.c_str(), 200, textColor, 12);
		fscreen->fCenterPrint(mainGame.c_str(), 700, textColor, 4);
		fscreen->fCenterPrint(limitTest.c_str(), 800, textColor, 4);
		fscreen->fCenterPrint(howToPlay.c_str(), 900, textColor, 4);
		fscreen->fCenterPrint(endScreen.c_str(), 1000, textColor, 2);

		DrawLights();
	}
	else if (game_state == HOW_TO_PLAY)
	{
		lightsToGoThrough = 4;
		string objective = "Objective: Eliminate all enemies!";
		fscreen->fCenterPrint(objective.c_str(), 100, textColor, 5);
		string enemyTypes = "There are three types";
		fscreen->fCenterPrint(enemyTypes.c_str(), 200, textColor, 4);
		string instructions = "Match your character color with that of the enemy";
		fscreen->fCenterPrint(instructions.c_str(), 700, textColor, 3);
		string wasd = "WASD to move";
		fscreen->fPrint(wasd.c_str(), 50, 450, textColor, 3);
		string shoot = "Left Mouse click to shoot";
		fscreen->fPrint(shoot.c_str(), 50, 500, textColor, 3);
		string disclaimer = "(Bullets won't hit in this showcase)";
		fscreen->fPrint(disclaimer.c_str(), 50, 525, textColor, 2);
		string map = "Hold TAB to show the map";
		fscreen->fPrint(map.c_str(), 50, 575, textColor, 3);
		string disclaimer1 = "(Map won't work in this showcase)";
		fscreen->fPrint(disclaimer1.c_str(), 50, 600, textColor, 3);
		string back = "(B) Back";
		fscreen->fPrint(back.c_str(), 1024 - 100, 1024 - 20, textColor, 2);
		string start = "(ENTER) Start";
		fscreen->fPrint(start.c_str(), 1024 - 163, 1024 - 40, textColor, 2);
		for (int i = 1; i < lightsToGoThrough; i++)
		{
			lights[i]->m_pos = ScreenToWorld(200.0f + 312.0f * (i-1), 800.0f, cam);
		}
		for (int i = 0; i < enemies.size(); i++)
		{
			enemies[i]->m_image->fCopyTo(fscreen, 406 + (10 + 64) * i, 300);
		}
		float2 playerScreenPos = WorldToScreen(player->m_pos.x, player->m_pos.y, cam);
		if (playerScreenPos.x <= 100)
			offset = 0.5f * deltaTime;
		else if (playerScreenPos.x >= 1024 - 100)
			offset = -0.5f * deltaTime;
		player->m_pos = ScreenToWorld(playerScreenPos.x + offset, 900, cam);

		DrawLights();
		player->Update(deltaTime, cam);
		ProcessInput();
		player->m_charging++;
		if (!projectiles.empty())
		{
			for (int i = 0; i < projectiles.size(); i++)
			{
				projectiles[i]->Draw(fscreen, cam);
			}
		}
		player->CalculateColor(fscreen);
		player->m_body->Draw(fscreen, cam);
	}

	for (int i = 0; i < polygons.size(); i++)
	{
		polygons[i]->Draw(fscreen, cam);
	}
}

void Game::InitGame(float& deltaTime)
{
	cam.Reset();
	rotation_modifier = 0.0f;
	for (auto it : lights)
	{
		delete it;
	}
	lights.clear();
	for (auto it : polygons)
	{
		delete it;
	}
	polygons.clear();
	for (auto it : projectiles)
	{
		delete it->m_body;
		delete it;
	}
	projectiles.clear();
	for (auto it : enemies)
	{
		delete it->m_body;
		delete it;
	};
	enemies.clear();
	delete player->m_body;
	delete player;
	for (auto it : mirrors)
	{
		delete it;
	}

	// Create world border
	vector<float> angles;
	int numSides = 4;
	//numSides = randomNum(3, 13);
	angles.push_back(45.0f);
	//angles.push_back(randomNum(0, 360 / numSides));
	for (int j = 1; j < numSides; j++)
	{
		angles.push_back(angles[j - 1] + 360 / numSides);
		//angles.push_back(randomNum(angles[j-1], 360));
	}
	Poly* aPoly = new Poly({ 0.0f, 0.0f }, 6144.0f, { 1.0f, 1.0f, 1.0f }, 0, numSides, angles, false);
	polygons.push_back(aPoly);
	angles.clear();

	// create enemies
	for (int i = 0; i < 30; i++)
	{
		int colorType = randomNum(0, 2);
		float3 color;
		if (colorType == 0)
			color = { 5.0f, 0.0f, 0.0f };
		else if (colorType == 1)
			color = { 0.0f, 5.0f, 0.0f };
		else
			color = { 0.0f, 0.0f, 5.0f };
		float2 pos = { static_cast<float>(randomNum(-WORLDWIDTH / 2, WORLDWIDTH / 2)), static_cast<float>(randomNum(-WORLDHEIGHT / 2, WORLDHEIGHT / 2)) };
		Circle* aBody = new Circle(pos, 32.0f, { 10.0f, 10.0f, 10.0f }, i+1);
		Enemy* aEnemy = new Enemy(pos, *aBody, color, 0.3f);
		enemies.push_back(aEnemy);
	}
	
	// create polygons
	float radius = 250.0f;
	//numSides = 10;
	for (int i = 0; i < 100; i++)
	{
		//angles.push_back(0.0f);
		numSides = randomNum(3, 8);
		angles.push_back(static_cast<float>(randomNum(0, 360 / numSides)));
		for (int j = 1; j < numSides; j++)
		{
			//angles.push_back(angles[j - 1] + 360 / numSides);
			angles.push_back(static_cast<float>(randomNum(static_cast<int>(angles[j-1]), 360)));
		}
		Poly* aPoly = new Poly({ static_cast<float>(randomNum(static_cast<int>(- WORLDWIDTH / 2 + radius / 2), static_cast<int>(WORLDWIDTH / 2 - radius / 2))), static_cast<float>(randomNum(static_cast<int>(-WORLDHEIGHT / 2 + radius / 2), static_cast<int>(WORLDHEIGHT / 2 - radius / 2)))}, radius, {static_cast<float>(randomNum(0, 10)) / 10.0f, static_cast<float>(randomNum(0, 10)) / 10.0f, static_cast<float>(randomNum(0, 10)) / 10.0f}, i + 1, numSides, angles, true);
		polygons.push_back(aPoly);
		angles.clear();
	}

	// Create character
	float2 playerPos = { static_cast<float>(randomNum(-WORLDWIDTH / 2 + 64 / 2, WORLDWIDTH / 2 - 64 / 2)), static_cast<float>(randomNum(-WORLDHEIGHT / 2 + 64 / 2, WORLDHEIGHT / 2 - 64 / 2)) };
	playerBody = new Circle(playerPos, 64.0f, { 10.0f, 10.0f, 10.0f }, 0);
	player = new Player(playerPos, *playerBody, 0.5f);
	bool flag = false;
	int cnt = 0;
	while (!flag)
	{
		cnt = 0;
		for (int i = 0; i < polygons.size(); i++)
		{
			if (player->PolygonIntersect(*polygons[i], deltaTime))
			{
				cnt++;
				printf("choosing new pos\n");
				player->m_pos = { static_cast<float>(randomNum(-WORLDWIDTH / 2 + 64 / 2, WORLDWIDTH / 2 - 64 / 2)), static_cast<float>(randomNum(-WORLDHEIGHT / 2 + 64 / 2, WORLDHEIGHT / 2 - 64 / 2)) };
				player->Update(deltaTime, cam);
				player->m_body->UpdateScreenPos(cam);
				break;
			}
			/*else
				flag = true;*/
			
		}
		if (cnt == 0)
			flag = true;
	}
	//addLightSource(player->m_pos, { 5.0f, 5.0f, 5.0f }, 0, 0);

	// create mirrors
	//vector<float> angles1;
	//numSides = 2;
	//for (int i = 0; i < 2; i++)
	//{
	//	//numSides = randomNum(3, 13);
	//	angles1.push_back(-50.0f);
	//	//angles.push_back(randomNum(0, 360 / numSides));
	//	for (int j = 1; j < numSides; j++)
	//	{
	//		angles1.push_back(angles1[j - 1] + 360 / numSides);
	//		//angles.push_back(randomNum(angles[j-1], 360));
	//	}
	//	Poly* aMirror = new Poly({ -50.0f + 250 * i, -150.0f }, 50.0f, { 5.0f, 0.0f, 0.0f }, i, numSides, angles1, false);
	//	mirrors.push_back(aMirror);
	//	angles1.clear();
	//}

	// create light sources
	float2 pos = { static_cast<float>(randomNum(-WORLDWIDTH / 2, WORLDWIDTH / 2)), static_cast<float>(randomNum(-WORLDHEIGHT / 2, WORLDHEIGHT / 2)) };
	float3 color = { static_cast<float>(randomNum(1, 15)), static_cast<float>(randomNum(1, 15)), static_cast<float>(randomNum(1, 15)) };
	addLightSource(pos, color, 0, 0);
	for (int i = 0; i < 45; i++)
	{
		pos = { static_cast<float>(randomNum(-WORLDWIDTH / 2, WORLDWIDTH / 2)), static_cast<float>(randomNum(-WORLDHEIGHT / 2, WORLDHEIGHT / 2)) };
		color = { static_cast<float>(randomNum(1, 15)), static_cast<float>(randomNum(1, 15)), static_cast<float>(randomNum(1, 15)) };
		float max = fmaxf(color.x, fmaxf(color.y, color.z));
		float colorPercent = 0.003f;
		float maxRadius = sqrtf(max / colorPercent) * 10.0f;

		float distance = 0.0f;
		bool flag = false;
		int cnt = 0;
		while (!flag)
		{
			for (int j = 0; j < lights.size(); j++)
			{
				distance = length(pos - lights[j]->m_pos);
				maxRadius = sqrtf(max / colorPercent) * 10.0f;
				if (distance < lights[j]->m_maxRadius + maxRadius / 5)
				{
					pos = { static_cast<float>(randomNum(-WORLDWIDTH / 2, WORLDWIDTH / 2)), static_cast<float>(randomNum(-WORLDHEIGHT / 2, WORLDHEIGHT / 2)) };
					color = { static_cast<float>(randomNum(1, 15)), static_cast<float>(randomNum(1, 15)), static_cast<float>(randomNum(1, 15)) };
					flag = false;
					break;
				}
				else
					flag = true;
			}
			cnt++;
			if (cnt == 5000)
				break;
			
		}
		if(cnt < 5000)
			addLightSource(pos, color, 0, i + 1);
	}

	lightsToGoThrough = static_cast<int>(lights.size());
}

void Game::MainGame(float& deltaTime)
{
	// Process Input
	ProcessInput();

	//Update player
	if (!showMap)
	{
		player->m_charging++;
		bool isIntersecting = false;
		for (int i = 1; i < polygons.size(); i++)
		{
			if (player->PolygonIntersect(*polygons[i], deltaTime))
			{
				isIntersecting = true;
				break;
			}
		}
		if (!isIntersecting)
		{
			player->Update(deltaTime, cam);
			//lights[0]->m_pos = player->m_pos;
		}
	}

	// Update Enemies
	if(!showMap)
		for (int i = 0; i < enemies.size(); i++)
		{
			enemies[i]->DetectPlayer(player->m_pos);
			enemies[i]->Update(deltaTime, cam);
			enemies[i]->Attack(*player);
		}

	// Update Projectiles
	if (!showMap)
	{
		if (!projectiles.empty())
		{
			for (int i = 0; i < projectiles.size(); i++)
			{
				if (projectiles[i]->m_lifetime <= 0)
				{
					delete (projectiles[i]);
					projectiles.erase(projectiles.begin() + i);
					i--;
					continue;
				}
				projectiles[i]->Update(deltaTime, cam);
				for (int j = 0; j < enemies.size(); j++)
				{
					projectiles[i]->Attack(*enemies[j]);
					if (enemies[j]->m_hp <= 0)
					{
						delete (enemies[j]);
						enemies.erase(enemies.begin() + j);
						j--;
						continue;
					}
				}
				projectiles[i]->m_lifetime--;
			}
		}
	}

	UpdateCamera(deltaTime);

	// [SCREEN SPACE]
	// A |--------------------------->| B
	//   |                            |
	//   |                            |
	//   |                            |
	//   |                            |
	//   ..............................
	// 
	// [WORLD SPACE]
	//   ...............................
	//   |                             |
	//   |    A -----                  |
	//   |           -----             |
	//   |                -----> B     |
	//   |                             |
	//   ...............................
	// plotting texture with lines
	// big thanks to Daniel & Amzy for helping me fix my algorithm and making the backdrop tiled!
	for (int y = 0; y < SCRHEIGHT; y++)
	{
		// texture Coordinates
		float2 A = ScreenToWorld(0.0f, static_cast<float>(y), cam);
		float2 B = ScreenToWorld(SCRWIDTH, static_cast<float>(y), cam);
		float2 AtoB = B - A;
		float2 deltaAtoB = float2(AtoB.x, AtoB.y) / SCRWIDTH;

		for (int x = 0; x < SCRWIDTH; x++)
		{
			int backdropCoordX = static_cast<int>(A.x + x * deltaAtoB.x);
			int backdropCoordY = static_cast<int>(A.y + x * deltaAtoB.y);

			//if (backdropCoordX >= 0.0f && backdropCoordY >= 0 && backdropCoordX <= backdrop->width && backdropCoordY <= backdrop->height)
			fscreen->pixels[x + y * SCRHEIGHT] = backdrop->pixels[(backdropCoordX & (backdrop->width - 1)) + (backdropCoordY & (backdrop->height - 1)) * backdrop->width];
		}
	}

	if (enemies.size() <= 20)
	{
		for (int i = 0; i < enemies.size(); i++)
		{
			fscreen->fLine(player->m_body->m_screenCenter.x, player->m_body->m_screenCenter.y, enemies[i]->m_body->m_screenCenter.x, enemies[i]->m_body->m_screenCenter.y, { 5.5f, 5.5f, 5.5f });
		}
	}

	player->m_body->UpdateScreenPos(cam);
	if (!showMap)
	{
		player->Draw(fscreen, cam);
	}
	
	if (!projectiles.empty())
	{
		for (int i = 0; i < projectiles.size(); i++)
		{
			projectiles[i]->Draw(fscreen, cam, 8.0f);
		}
	}

	if (!showMap)
	{
		#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < enemies.size(); i++)
		{
			enemies[i]->m_body->UpdateScreenPos(cam);
			enemies[i]->Draw(fscreen, cam, 2.0f);
			//enemies[i]->m_image->fCopyTo(fscreen, static_cast<int>(enemies[i]->m_body->m_screenCenter.x), static_cast<int>(enemies[i]->m_body->m_screenCenter.y));
		}
	}

	// light up the screen
	DrawLights();

	if (showMap)
	{
		float2 playerMapPos = WorldToScreen(playerTempPos.x, playerTempPos.y, cam);
		fscreen->fBar(static_cast<int>(playerMapPos.x - player->m_body->m_screenRadius), static_cast<int>(playerMapPos.y - player->m_body->m_screenRadius),
			static_cast<int>(playerMapPos.x + player->m_body->m_screenRadius), static_cast<int>(playerMapPos.y + player->m_body->m_screenRadius),
			{ 10.0f, 10.0f, 10.0f });
	}

	player->CalculateColor(lightOnlySurface);
	player->m_body->Draw(fscreen, cam);

	// Draw primitives
	for (int i = 0; i < polygons.size(); i++)
	{
		polygons[i]->Draw(fscreen, cam);
	}
	for (int i = 0; i < mirrors.size(); i++)
	{
		mirrors[i]->Draw(fscreen, cam);
	}

	DrawUI();

	if (player->m_hp <= 0)
	{
		game_state = END_SCREEN;
		InitEndScreen();
	}
	if (enemies.empty())
	{
		game_state = END_SCREEN;
		victory = true;
		InitEndScreen();
	}
}

void Game::InitStressTest()
{
	cam.Reset();
	rotation_modifier = 0.0f;
	for (auto it : lights)
	{
		delete it;
	}
	lights.clear();
	for (auto it : polygons)
	{
		delete it;
	}
	polygons.clear();
	for (auto it : projectiles)
	{
		delete it->m_body;
		delete it;
	}
	projectiles.clear();
	for (auto it : enemies)
	{
		delete it->m_body;
		delete it;
	};
	enemies.clear();
	delete player->m_body;
	delete player;

	//cam.m_scale = 0.125;
	cam.m_scale = 0.0625;

	// Create world border
	vector<float> angles;
	int numSides = 4;
	//numSides = randomNum(3, 13);
	angles.push_back(45.0f);
	//angles.push_back(randomNum(0, 360 / numSides));
	for (int j = 1; j < numSides; j++)
	{
		angles.push_back(angles[j - 1] + 360 / numSides);
		//angles.push_back(randomNum(angles[j-1], 360));
	}
	Poly* aPoly = new Poly({ 0.0f, 0.0f }, 5900.0f, { 10.0f, 10.0f, 10.0f }, 0, numSides, angles, false);
	polygons.push_back(aPoly);
	angles.clear();

	// Create character
	playerBody = new Circle({ 0.0f, 0.0f }, 64.0f, { 10.0f, 10.0f, 10.0f }, 0);
	player = new Player({ -3600.0f, -3600.0f }, *playerBody, 0.5f);
	//addLightSource(player->m_pos, { 5.0f, 5.0f, 5.0f }, 0, 0);

	// create polygons
	float radius = 50.0f;
	numSides = 10;
	//numSides = randomNum(3, 8);
	for (int i = 0; i < 8000; i++)
	{
		//numSides = randomNum(3, 13);
		//angles.push_back(0.0f);
		angles.push_back(static_cast<float>(randomNum(0, 360 / numSides)));
		for (int j = 1; j < numSides; j++)
		{
			angles.push_back(angles[j - 1] + 360 / numSides);
			//angles.push_back(randomNum(angles[j - 1], 360));
		}
		Poly* aPoly = new Poly({ static_cast<float>(randomNum(static_cast<int>(-WORLDWIDTH / 2 + radius / 2), static_cast<int>(WORLDWIDTH / 2 - radius / 2))), static_cast<float>(randomNum(static_cast<int>(-WORLDHEIGHT / 2 + radius / 2), static_cast<int>(WORLDHEIGHT / 2 - radius / 2))) }, radius, { static_cast<float>(randomNum(0, 10)) / 10.0f, static_cast<float>(randomNum(0, 10)) / 10.0f, static_cast<float>(randomNum(0, 10)) / 10.0f }, i + 1, numSides, angles, true);
		//Poly* aPoly = new Poly({ static_cast<float>(randomNum(-WORLDWIDTH/2 + radius/2, WORLDWIDTH/2 - radius/2)), static_cast<float>(randomNum(-WORLDHEIGHT / 2 + radius / 2, WORLDHEIGHT / 2 - radius / 2))}, radius, { 2.0f, 2.0f, 2.0f }, i + 1, numSides, angles, false);
		polygons.push_back(aPoly);
		angles.clear();
	}

	// create light sources
	float2 pos = { 0.0f, 0.0f };
	float3 color = { 25.0f, 15.0f, 50.0f };
	//float3 color = { 250.0f, 150.0f, 500.0f };
	addLightSource(pos, color, 0, 0);

	lightsToGoThrough = static_cast<int>(lights.size());
}

void Game::StressTest(float& deltaTime)
{
	player->Update(deltaTime, cam);

	UpdateCamera(deltaTime);

	lights[0]->m_pos = ScreenToWorld(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y), cam);

	// light up the screen
	DrawLights();

	player->CalculateColor(lightOnlySurface);
	player->m_body->Draw(fscreen, cam);

	// Draw primitives
	for (int i = 0; i < polygons.size(); i++)
	{
		polygons[i]->Draw(fscreen, cam);
	}
}

void Game::InitEndScreen()
{
	cam.Reset();
	rotation_modifier = 0.0f;
	for (auto it : lights)
	{
		delete it;
	}
	lights.clear();
	for (auto it : polygons)
	{
		delete it;
	}
	polygons.clear();
	for (auto it : projectiles)
	{
		delete it->m_body;
		delete it;
	}
	projectiles.clear();
	for (auto it : enemies)
	{
		delete it->m_body;
		delete it;
	};
	enemies.clear();
	delete player->m_body;
	delete player;

	lightsToGoThrough = 1;
	playerBody = new Circle({ 0.0f, 0.0f }, 64.0f, { 10.0f, 10.0f, 10.0f }, 0);
	player = new Player({ 0.0f, 0.0f }, *playerBody, 0.5f);

	// create enemies
	for (int i = 0; i < 3; i++)
	{
		int colorType = i;
		float3 color;
		if (colorType == 0)
			color = { 2.0f, 0.0f, 0.0f };
		else if (colorType == 1)
			color = { 0.0f, 2.0f, 0.0f };
		else
			color = { 0.0f, 0.0f, 2.0f };
		float2 pos = { -74.0f + (64 + 10) * i, -100.0f };
		Circle* aBody = new Circle(pos, 32.0f, { 10.0f, 10.0f, 10.0f }, i + 1);
		Enemy* aEnemy = new Enemy(pos, *aBody, color, 0.3f);
		enemies.push_back(aEnemy);
	}


	// create polygons
	//float radius = 150.0f;
	//vector<float> angles;
	//int numSides = 4;
	//for (int i = 0; i < 3; i++)
	//{
	//	//numSides = randomNum(3, 13);
	//	//angles.push_back(0.0f);
	//	angles.push_back(static_cast<float>(randomNum(0, 360 / numSides)));
	//	for (int j = 1; j < numSides; j++)
	//	{
	//		angles.push_back(angles[j - 1] + 360 / numSides);
	//		//angles.push_back(randomNum(angles[j-1], 360));
	//	}
	//	Poly* aPoly = new Poly({ static_cast<float>(randomNum(static_cast<int>(-SCRWIDTH / 2 + radius / 2), static_cast<int>(0 - radius / 2))), static_cast<float>(randomNum(static_cast<int>(-SCRHEIGHT / 2 + radius / 2), static_cast<int>(0 - radius / 2))) }, radius, { static_cast<float>(randomNum(0, 10)) / 10.0f, static_cast<float>(randomNum(0, 10)) / 10.0f, static_cast<float>(randomNum(0, 10)) / 10.0f }, i + 1, numSides, angles, true);
	//	polygons.push_back(aPoly);
	//	angles.clear();
	//}
	// create mirrors
	float3 mirrorColor = { 1.0f, 1.0f, 1.0f };
	if (victory)
	{
		mirrorColor = { 0.0f, 5.0f, 0.0f };
	}
	else
	{
		mirrorColor = { 5.0f, 0.0f, 0.0f };
	}
	vector<float> angles1;
	float radius = 50.0f;
	int numSides = 2;
	for (int i = 0; i < 5; i++)
	{
		//numSides = randomNum(3, 13);
		angles1.push_back(-50.0f);
		//angles.push_back(randomNum(0, 360 / numSides));
		for (int j = 1; j < numSides; j++)
		{
			angles1.push_back(angles1[j - 1] + 360 / numSides);
			//angles.push_back(randomNum(angles[j-1], 360));
		}
		//Poly* aMirror = new Poly({ -50.0f + 250 * i, -150.0f }, 50.0f, { 5.0f, 0.0f, 0.0f }, i, numSides, angles1, false);
		Poly* aMirror = new Poly({ static_cast<float>(randomNum(static_cast<int>(-SCRWIDTH / 2 + radius / 2), static_cast<int>(SCRWIDTH / 2 - radius / 2))), static_cast<float>(randomNum(static_cast<int>(-SCRHEIGHT / 2 + radius / 2), static_cast<int>(SCRHEIGHT / 2 - radius / 2))) }, radius, mirrorColor, i + 1, numSides, angles1, true);
		mirrors.push_back(aMirror);
		angles1.clear();
	}

	float3 color = { 1.0f, 1.0f, 1.0f };
	if (victory)
	{
		color = { 8.0f, 15.0f, 5.0f };
	}
	else
	{
		color = { 15.0f, 1.0f, 1.0f };
	}
	addLightSource({ 0.0f, 0.0f }, color, 0, 0);
	//addLightSource({ 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f }, 0, 1);
}

void Game::EndScreen(float& deltaTime)
{
	rotation_modifier = 0.02f * deltaTime;
	UpdateCameraMainMenu(deltaTime);

	lights[0]->m_pos = ScreenToWorld(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y), cam);

	string endText = "";
	float3 textColor = { 10.0f, 10.0f, 10.0f };
	if (victory)
	{
		endText = "VICTORY";
	}
	else
	{
		endText = "DEFEAT";
	}
	fscreen->fCenterPrint(endText.c_str(), 200, textColor, 16);

	string playAgain = "Press ENTER to play again";
	fscreen->fCenterPrint(playAgain.c_str(), 800, textColor, 4);
	string exit = "Press ESC to quit";
	fscreen->fCenterPrint(exit.c_str(), 900, textColor, 4);

	DrawLights();

	for (int i = 0; i < mirrors.size(); i++)
	{
		mirrors[i]->Draw(fscreen, cam);
	}
}

void Game::FPSCounter(const float& deltaTime)
{
	int frameCnt = static_cast<int>(1000.0f / deltaTime);
	string fps = "FPS: " + to_string(frameCnt);
	fscreen->fPrint(fps.c_str(), 5, 5, {10.0f, 10.0f, 0.0f}, 1);
	deltaTimeSum += deltaTime;
	if (deltaTimeSum >= 1000.0f)
	{
		printf("%i FPS\n", frameCnt);
		deltaTimeSum = 0;
	}
}

void Game::UpdateCameraMainMenu(const float& deltaTime)
{
	cam.m_coord = float2(-SCRWIDTH / 2, -SCRHEIGHT / 2);
	//cam.m_coord += camTempOffset * deltaTime;
	float2 mouseWSBeforeZoom = ScreenToWorld(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y), cam); //WS stands for World Space
	cam.m_scale *= scale_modifier;
	mouseInWorldSpace = ScreenToWorld(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y), cam);

	cam.m_coord.x += (mouseWSBeforeZoom.x - mouseInWorldSpace.x);
	cam.m_coord.y += (mouseWSBeforeZoom.y - mouseInWorldSpace.y);

	cam.m_angle += rotation_modifier;

	// setup rotation coordinate system
	float a_angle = DEG_TO_RAD(cam.m_angle);
	cam.A.x = cosf(a_angle);
	cam.A.y = sinf(a_angle);

}

void Game::UpdateCamera(const float& deltaTime)
{
	cam.m_coord = player->m_pos - float2( SCRWIDTH / 2, SCRHEIGHT / 2);
	//cam.m_coord += camTempOffset * deltaTime;
	float2 mouseWSBeforeZoom = ScreenToWorld(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y), cam); //WS stands for World Space
	cam.m_scale *= scale_modifier;
	mouseInWorldSpace = ScreenToWorld(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y), cam);

	cam.m_coord.x += (mouseWSBeforeZoom.x - mouseInWorldSpace.x);
	cam.m_coord.y += (mouseWSBeforeZoom.y - mouseInWorldSpace.y);

	cam.m_angle += rotation_modifier;

	// setup rotation coordinate system
	float a_angle = DEG_TO_RAD(cam.m_angle);
	cam.A.x = cosf(a_angle);
	cam.A.y = sinf(a_angle);

}

void Game::DrawLights()
{
	float2 mouseWorldPos = ScreenToWorld(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y), cam);

	for (int i = 0; i < lights.size(); i++)
	{
		lights[i]->Update(cam);
	}
	for (int i = 0; i < mirrors.size(); i++)
		mirrors[i]->UpdateScreenPos(cam);
	for (int i = 0; i < polygons.size(); i++)
		polygons[i]->UpdateScreenPos(cam);
	/*for (int i = 0; i < circles.size(); i++)
		circles[i]->UpdateScreenPos(cam);*/

	#pragma omp parallel for schedule(dynamic)
	// with shadow grid
	// D    C
	//   ___
	//  |   |
	//  |   |
	//   ___
	// A     B
	for (int i = 0; i < shadowGrid.cells.size(); i++)
	{
		/*float3 A = { 0.01f, 0.01f, 0.01f },
			B = { 0.01f, 0.01f, 0.01f },
			C = { 0.01f, 0.01f, 0.01f },
			D = { 0.01f, 0.01f, 0.01f };
		if (game_state == MAIN_MENU || game_state == END_SCREEN)
		{
			A = { 0.0f, 0.0f, 0.0f };
			B = { 0.0f, 0.0f, 0.0f };
			C = { 0.0f, 0.0f, 0.0f };
			D = { 0.0f, 0.0f, 0.0f };
		}*/
		float3 D = GetPixelColor(static_cast<int>(shadowGrid.cells[i].x), static_cast<int>(shadowGrid.cells[i].y));
		float3 C = GetPixelColor(static_cast<int>(shadowGrid.cells[i].x + shadowGrid.cellSideSize), static_cast<int>(shadowGrid.cells[i].y));
		float3 A = GetPixelColor(static_cast<int>(shadowGrid.cells[i].x), static_cast<int>(shadowGrid.cells[i].y + shadowGrid.cellSideSize));
		float3 B = GetPixelColor(static_cast<int>(shadowGrid.cells[i].x + shadowGrid.cellSideSize), static_cast<int>(shadowGrid.cells[i].y + shadowGrid.cellSideSize));
		float marginOfError = 0.03f;

		if(fabs((A.x + A.y + A.z) - (B.x + B.y + B.z)) < marginOfError
		&& fabs((A.x + A.y + A.z) - (C.x + C.y + C.z)) < marginOfError
		&& fabs((A.x + A.y + A.z) - (D.x + D.y + D.z)) < marginOfError
		&& fabs((B.x + B.y + B.z) - (C.x + C.y + C.z)) < marginOfError
		&& fabs((B.x + B.y + B.z) - (D.x + D.y + D.z)) < marginOfError
		&& fabs((C.x + C.y + C.z) - (D.x + D.y + D.z)) < marginOfError)
		{
			// the 4 corners of the grid are similar. Each pixel in the cell has the same color.
			for (int y = static_cast<int>(shadowGrid.cells[i].y); y < static_cast<int>(shadowGrid.cells[i].y + shadowGrid.cellSideSize); y++)
			{
				for (int x = static_cast<int>(shadowGrid.cells[i].x); x < static_cast<int>(shadowGrid.cells[i].x + shadowGrid.cellSideSize); x++)
				{
					//fscreen->pixels[x + y * SCRWIDTH] = { 0.0f, 0.0f, 0.1f };
					// Rareș suggested the general idea to multiply the light color by the current pixel.
					fscreen->pixels[x + y * SCRWIDTH] = fscreen->pixels[x + y * SCRWIDTH] * A/* + float3(0.05f, 0.05f, 0.05f)*/; //this commented part adds ambient light
					lightOnlySurface->pixels[x + y * SCRWIDTH] = A;
				}
			}
		}
		else
		{
			// the 4 corners of the grid are not similar. Iterate through the cell normally.
			for (int y = static_cast<int>(shadowGrid.cells[i].y); y < static_cast<int>(shadowGrid.cells[i].y + shadowGrid.cellSideSize); y++)
			{
				for (int x = static_cast<int>(shadowGrid.cells[i].x); x < static_cast<int>(shadowGrid.cells[i].x + shadowGrid.cellSideSize); x++)
				{
					/*float3 color = { 0.01f, 0.01f, 0.01f };
					if (game_state == MAIN_MENU || game_state == END_SCREEN)
						color = { 0.0f, 0.0f, 0.0f };*/
					float3 color = GetPixelColor(x, y);
					// lights the shapes as well as the "floor"
					// Rareș suggested the general idea to multiply the light color by the current pixel.
					fscreen->pixels[x + y * SCRWIDTH] = fscreen->pixels[x + y * SCRWIDTH] * color;
					lightOnlySurface->pixels[x + y * SCRWIDTH] = color;
				}
			}
		}
	}
}

float3 Game::GetPixelColor(int x, int y)
{
	float3 color = { 0.02f, 0.02f, 0.02f };
	if (game_state == MAIN_MENU/* || game_state == END_SCREEN*/)
		color = { 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < lightsToGoThrough; i++)
	{
		bool next = false;
		//lights[i]->Draw(x, y, mousePos, color); // 5-6 less FPS when I abstract it in a function
		float2 vec = float2(x - lights[i]->m_screenPos.x, y - lights[i]->m_screenPos.y);
		float vecLength = length(vec);
		float radius = vecLength / lights[i]->m_intensity;
		if (radius >= lights[i]->m_maxRadius/* * cam.m_scale*/)
			continue;

		// circle intersections
		if(game_state == GAME)
			if(player->m_body->AABBCheck(lights[i]->m_screenPos, float2(static_cast<float>(x), static_cast<float>(y))))
				if (player->m_body->Intersect(vec, lights[i]->m_screenPos)) continue;

		// polygon intersections
		for (int j = 0; j < polygons.size(); j++)
		{
			// if there's no AABB collision this polygon is not relevant
			if (!polygons[j]->AABBCheck(lights[i]->m_screenPos, float2(static_cast<float>(x), static_cast<float>(y)))) continue;

			if (polygons[j]->Intersect(vec, lights[i]->m_screenPos))
			{
				next = true;
				break;
			}
		}
		if (next) continue;

		// mirror intersections
		for (int j = 0; j < mirrors.size(); j++)
		{
			//if (!mirrors[j]->AABBCheck(lights[i]->m_screenPos, float2(x, y))) continue;

			if (mirrors[j]->MirrorIntersect(float2(x, y), *lights[i], polygons, color))
			{
				//next = true;
				//break;
			}
		}
		for (int j = 0; j < mirrors.size(); j++)
		{
			// if there's no AABB collision this polygon is not relevant
			if (!mirrors[j]->AABBCheck(lights[i]->m_screenPos, float2(x, y))) continue;

			if (mirrors[j]->Intersect(vec, lights[i]->m_screenPos))
			{
				next = true;
				break;
			}
		}
		if (next) continue;


		float inv_radius = 1 / radius;
		// spot lights
		if (lights[i]->m_type == 1)
		{
			float2 lightDir = float2(mousePos.x - lights[i]->m_screenPos.x, mousePos.y - lights[i]->m_screenPos.y);
			float2 normLightDir = normalize(lightDir);
			float2 normVec = normalize(vec);
			float dotProduct = dot(normVec, normLightDir);
			// smooth-edge
			float i_r = 0.9f; // inner radius
			float o_r = 0.75; // outer radius
			if (dotProduct > i_r)
				color += lights[i]->m_color * inv_radius;
			else if (dotProduct > o_r)
				color += lerp(lights[i]->m_color, float3(0.0f, 0.0f, 0.0f), (0.9f - dotProduct) / (i_r - o_r)) * inv_radius;
		}

		// point lights
		if (lights[i]->m_type == 0)
		{
			if (radius == 0.0f)
				radius = 1.0f;
			color += lights[i]->m_color * inv_radius;
		}
	}
	return color;
}

void Game::addLightSource(const float2& pos, const float3& color, const int& type, const int& id)
{
	Light* aLight = new Light(pos, color, type, id);
	if (aLight->m_type == SPOT_LIGHT)
	{
		spotlightIds.push_back(aLight->m_id);
		spotlightNormDir.push_back({ 0.0f, 0.0f });
	}
	lights.push_back(aLight);
}

void Game::CreateMirror(const float2& pos)
{
	// create mirrors
	vector<float> angles1;
	int numSides = 2;
	//numSides = randomNum(3, 13);
	angles1.push_back(-50.0f);
	//angles.push_back(randomNum(0, 360 / numSides));
	for (int j = 1; j < numSides; j++)
	{
		angles1.push_back(angles1[j - 1] + 360 / numSides);
		//angles.push_back(randomNum(angles[j-1], 360));
	}
	Poly* aMirror = new Poly(pos, 50.0f, { 5.0f, 0.0f, 0.0f }, 0, numSides, angles1, false);
	mirrors.push_back(aMirror);
	angles1.clear();
}

bool releaseLeft = true;
bool releaseRight = true;
void Game::ProcessInput()
{
	if (mouseLeft /*&& releaseLeft*/ && player->m_cooldown < player->m_charging)
	{
		player->m_charging = 0;
		Circle* aCircle = new Circle(player->m_pos, 16, player->m_weaponColor, 10);
		Projectile* aProj = new Projectile(player->m_pos, *aCircle, player->m_weaponColor, 0.6f);
		float2 directionVec = (ScreenToWorld(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y), cam) - player->m_pos);
		aProj->m_direction = normalize(directionVec);
		projectiles.push_back(aProj);
		releaseLeft = false;
	}
	if (!mouseLeft)
	{
		releaseLeft = true;
	}

}

void Game::DrawUI()
{
	string enemiesLeft = "Enemies Left: " + to_string(enemies.size());
	fscreen->fPrint(enemiesLeft.c_str(), 1024 - 300, 25, {1.0f, 1.0f, 0.0f}, 3);

	fscreen->fBar(25, 25, player->m_maxHp * 25, 50, { 10.0f, 0.0f, 0.0f });
	fscreen->fBar(25, 25, player->m_hp * 25, 50, { 0.0f, 10.0f, 0.0f });
}