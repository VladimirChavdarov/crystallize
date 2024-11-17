// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/UU - Jacco Bikker - 2006-2023

#pragma once
#include "ShadowGrid.h"
#include "Light.h"
#include "Entity.h"

enum States
{
	MAIN_MENU,
	HOW_TO_PLAY,
	STRESS_TEST,
	GAME,
	END_SCREEN
};

namespace Tmpl8
{

class Game : public TheApp
{
public:
	// game flow methods
	void Init();
	void Tick( float deltaTime );
	void Shutdown() { /* implement if you want to do something on exit */ }
	// input handling
	void MouseUp(int button);
	void MouseDown(int button);
	void MouseMove( int x, int y ) { mousePos.x = x, mousePos.y = y; }
	void MouseWheel(float y);
	void KeyUp(int key);
	void KeyDown(int key);
	// data members
	float m_deltaTime = 0;
	int game_state = MAIN_MENU;
	bool victory = false;
	int lightsToGoThrough = 0;
	int2 mousePos;
	bool mouseLeft = false;
	bool mouseRight = false;
	float2 mouseInWorldSpace;
	float wheel = 1.0f;
	float deltaTimeSum = 0;
	Camera cam = { { -SCRWIDTH / 2, -SCRHEIGHT / 2 }, 1.0f, 0.0f };
	float2 camTempOffset = { 0.0f, 0.0f }; // calculates with how much the camera will change it's position
	float scale_modifier = 1.0f;
	float rotation_modifier = 0.0f;
	// rotation matrix values
	float2 A = { 10.0f, 10.0f };
	fSurface* backdrop = new fSurface("assets/cobblestone_small.png");
	float2 backdropCoord = { -1024.0f, -1024.0f };

	bool showMap = false;
	bool mReleased = true;

	// game objects
	vector<Poly*> polygons;
	vector<Poly*> mirrors;
	//vector<Circle*> circles;
	vector<Light*> lights;
	vector<float2> spotlightNormDir;
	vector<int> spotlightIds;
	ShadowGrid shadowGrid = {8};

	// player 
	Player* player;
	float2 playerTempPos = { 0.0f, 0.0f };
	Circle* playerBody;
	vector<Projectile*> projectiles;

	// enemies
	vector<Enemy*> enemies;
	

	// ----------- Helper functions -----------
	void InitMainMenu();
	void InitStressTest();
	void InitGame(float& deltaTime);
	void InitEndScreen();
	void MainMenu(float& deltaTime);
	void StressTest(float& deltaTime);
	void MainGame(float& deltaTime);
	void EndScreen(float& deltaTime);
	void UpdateCamera(const float& deltaTime);
	void UpdateCameraMainMenu(const float& deltaTime);
	void FPSCounter(const float& deltaTime);
	void ProcessInput();
	void DrawUI();

	void DrawLights();
	float3 GetPixelColor(int x, int y);
	void addLightSource(const float2& pos, const float3& color, const int& type, const int& id);
	void CreatePolygon();
	void CreateMirror(const float2& pos);

	void PlotTexture(float2 imagePos, fSurface* image, fSurface* fscreen);
};

} // namespace Tmpl8