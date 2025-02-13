#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Core512.h"
#include <hgesprite.h>
#include "Input.h"
#include "Ship.h"
#include "Body.h"
#include "DynBody.h"
#include "TextOut.h"
#include "Music.h"
#include "Background.h"
#include "Explosion.h"

void CalculateAppSize(int& Width, int& Height);
void CoreLoad();
void CoreInit();
void CoreInput();
void CoreUnload();
bool CoreUpdate();
bool CoreRender();

HGE* exHGE;
Config exConfig;
Resources exResources;

const char* HelpText = "Core512 is best.\n\nControls :\n- Thrust -> Up Arrow / Down Arrow / Mouse Buttons\n- Rotate -> Left Arrow / Right Arrow / D / F\n- Reset Ship -> Space\n- Toggle Background -> T\n- Explode -> X\n- Quit -> Escape";

HTEXTURE hDefaultBodyTexture = NULL;
HTEXTURE hShipTexture = NULL;
Ship* lpShip = NULL;
Body* lpBody = NULL;
DynBody* lpDynBody = NULL;
hgeSprite* lpSprite = NULL;
Font* lpFont = NULL;
Music* lpMusic = NULL;
Background* lpBackground = NULL;

Vertex ShipInitialPosition;
Vertex ScrollOffset;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	exConfig.ReadFileINI();
	if(!exConfig.Width || !exConfig.Height)
		CalculateAppSize(exConfig.Width, exConfig.Height);

	exHGE = hgeCreate(HGE_VERSION);

	exHGE->System_SetState(HGE_TITLE, "Core512");
	exHGE->System_SetState(HGE_HIDEMOUSE, false);
	exHGE->System_SetState(HGE_SHOWSPLASH, false);
	exHGE->System_SetState(HGE_USESOUND, true);

	#ifdef _DEBUG
		exHGE->System_SetState(HGE_LOGFILE, "Core512.Log.txt");
	#endif

	exHGE->System_SetState(HGE_WINDOWED, true);
	exHGE->System_SetState(HGE_SCREENWIDTH, exConfig.Width);
	exHGE->System_SetState(HGE_SCREENHEIGHT, exConfig.Height);
	exHGE->System_SetState(HGE_SCREENBPP, 32);
	exHGE->System_SetState(HGE_FPS, HGEFPS_VSYNC);

	exHGE->System_SetState(HGE_FRAMEFUNC, CoreUpdate);
	exHGE->System_SetState(HGE_RENDERFUNC, CoreRender);

	exHGE->System_Initiate();

	CoreLoad();
	CoreInit();

	exHGE->System_Start();

	CoreUnload();

	exHGE->System_Shutdown();
	exHGE->Release();

	return 0;
}

//Size is 75% of screen size
void CalculateAppSize(int& Width, int& Height)
{
	Width = int(GetSystemMetrics(SM_CXSCREEN) * 0.75);
	Height = int(GetSystemMetrics(SM_CYSCREEN) * 0.75);
}

void CoreLoad()
{
	hDefaultBodyTexture = exResources.GetTexture("Res\\Body.png")->hTexture;
	hShipTexture = exResources.GetTexture("Res\\Ship.png")->hTexture;

	lpShip = new Ship(hShipTexture);

	lpBody = new Body(Vertex(200, 200), hDefaultBodyTexture);
	lpDynBody = new DynBody(Vertex(220, 150), hDefaultBodyTexture);

	lpSprite = new hgeSprite(hDefaultBodyTexture, 0, 0, 16, 16);
	lpSprite->SetColor(0xFFFF0000);

	lpFont = new Font();
	lpMusic = new Music();
	lpBackground = new Background();
}

void CoreInit()
{
	ShipInitialPosition.x = float(exConfig.Width >> 1);
	ShipInitialPosition.y = float(exConfig.Height >> 1);
	lpShip->Move(ShipInitialPosition.x, ShipInitialPosition.y);

	lpMusic->Play();

	MessageBox(exHGE->System_GetState(HGE_HWND), HelpText, "Core512", MB_OK | MB_ICONINFORMATION);
}

void CoreUnload()
{
	DeleteNull(lpShip)
	DeleteNull(lpBody)
	DeleteNull(lpDynBody)
	DeleteNull(lpSprite)
	DeleteNull(lpFont)
	DeleteNull(lpMusic)
	DeleteNull(lpBackground);
}

void CoreInput()
{
	int Command = InputGetCommand();

	if(Command == CMD_SHIP_RESET)
	{
		lpShip->Move(ShipInitialPosition.x, ShipInitialPosition.y);
		lpShip->HardStop();
	}

	if(Command == CMD_SHIP_EXPLODE)
		lpShip->Explode();

	if(Command == CMD_BACKGROUND_TOGGLE)
		lpBackground->Toggle();
}

bool CoreUpdate()
{
	if(exHGE->Input_GetKeyState(HGEK_ESCAPE))
		return true;

	CoreInput();

	Vertex Force(1.0f, 0.0f);
	lpDynBody->ApplyForce(Force);
	lpDynBody->RotationOffset(0.1f);
	lpDynBody->Update();

	int ForceDirection, RotationDirection;
	InputGetDirection(ForceDirection, RotationDirection);

	lpShip->Turn(RotationDirection);
	lpShip->Thrust(ForceDirection);
	lpShip->Update();

	ScrollOffset.x = lpShip->Rect.cx - ShipInitialPosition.x;
	ScrollOffset.y = lpShip->Rect.cy - ShipInitialPosition.y;

	return false;
}

bool CoreRender()
{
	exHGE->Gfx_BeginScene();
	exHGE->Gfx_Clear(ARGB(0xFF, 0xFF, 0xFF, 0xFF));

	lpBackground->RenderMosaic(exConfig.Width, exConfig.Height, ScrollOffset);
	lpBody->Render();
	lpDynBody->Render();
	lpSprite->Render(10, 0x20);
	lpShip->Render();
	lpFont->Render(8, 8, HelpText);
	lpFont->RenderFPS(8, 250);
	exHGE->Gfx_EndScene();

	return false;
}
