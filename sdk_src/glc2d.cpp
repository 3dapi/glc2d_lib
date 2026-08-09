#pragma warning(disable: 4996)

//#pragma comment(lib, "legacy_stdio_definitions.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dxguid.lib")

//#pragma comment(lib, "d3dx9.lib")
//#pragma comment(lib, "DxErr.lib")

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")

#include <chrono>
#include <string>
#include <windows.h>

#include "glcInternal.h"
#include "ILcInput.h"
#include "ILcFont.h"
#include "ILcTexture.h"
#include "ILcSpriteX.h"
#include "ILcSound.h"
#include "ILcModel.h"

#include "DsUtil.h"
#include "glc2d.h"
#include "d3dapp.h"

using std::chrono::duration_cast;
using std::chrono::steady_clock;
using std::chrono::milliseconds;

namespace glc
{
bool				m_bActive = true;
int					m_bCursor = TRUE;
int					m_bState  = TRUE;

HINSTANCE			m_hInst		{};
HWND				m_hWnd		{};
char				m_sCls		[512];
HICON				m_hIcon		{};
HACCEL				m_hAccel	{};


POINT				m_scnPos	{};
SIZE				m_scnSize	{640, 480};
float				m_scnScale	{1.2F};
BOOL				m_fullMode	{FALSE};
DWORD				m_dWinStyle	= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU| WS_VISIBLE;

bool					m_bWindowed		{true};
bool					m_bToggledEvent	{false};
bool					m_bToggledWin	{m_bWindowed};
bool					m_bDeviceLost	{};
LPDIRECT3D9				m_pD3D			{};			// D3D
D3DPRESENT_PARAMETERS	m_d3dppWin		{};
D3DPRESENT_PARAMETERS	m_d3dppFull		{};
LPDIRECT3DDEVICE9		m_pd3dDevice	{};		// Device
ILcSpriteX*				m_pLcSprite		{};		// 2D Sprite
LPD3DXSPRITE			m_pd3dSprite	{};		// DX Sprite for font draw
D3DXMATRIX				m_tmSprite	{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};	
DWORD						m_dColor	= 0xFF006699;
steady_clock::time_point	m_timeBgn	= steady_clock::now();
long long					m_timeElapsed=0;
ILcInput*					m_pInput;

int (*g2_FrameMove2D)();
int (*g2_Render2D)();

int (*g2_Keyboard)(uint8_t* key);
int (*g2_Mouse)(int x, int y, int z, int _event);

}// namespace glc

int g2_DefaultFrameMove()	{	return 0;	}
int g2_DefaultRender()		{	return 0;	}
int g2_DefaultKeyboard(uint8_t*){	return 0;	}
int g2_DefaultMouse(int, int, int, int){	return 0;	}

void g2_SetFrameMove(int (*v)() )
{
	glc::g2_FrameMove2D = v;
}

void g2_SetRender(int (*v)() )
{
	glc::g2_Render2D = v;
}

void g2_SetKeyboard(int (*v)(uint8_t*) )
{
	glc::g2_Keyboard	= v;
}

void g2_SetMouse(int (*v)(int, int, int, int) )
{
	glc::g2_Mouse = v;
}

const KEYCODE* g2_GetKeyboard()
{
	return glc::m_pInput->GetKeyMap();
}

int	g2_GetMouseX()
{
	const float* vcPos = glc::m_pInput->GetMousePos();
	return int(vcPos[0]);
}

int	g2_GetMouseY()
{
	const float* vcPos = glc::m_pInput->GetMousePos();
	return int(vcPos[1]);
}

int g2_GetMouseZ()
{
	const float* vcPos = glc::m_pInput->GetMousePos();
	return int(vcPos[2]);
}

VEC3i g2_GetMouse()
{
	const float* vcPos = glc::m_pInput->GetMousePos();
	return VEC3i{int(vcPos[0]), int(vcPos[1]), int(vcPos[2])};
}

int g2_GetMouseEvent(int nMouse)
{
	int	nState = glc::m_pInput->BtnState(nMouse);
	return nState;
}

DWORD	g2_GetWindowStyle()				{	return glc::m_dWinStyle;			}
HWND	g2_GetHwnd()					{	return glc::m_hWnd;					}
int		g2_GetScnW()					{	return glc::m_scnSize.cx;			}
int		g2_GetScnH()					{	return glc::m_scnSize.cy;			}
float	g2_GetScnScale()				{	return glc::m_scnScale;				}
bool	g2_GetWindowMode()				{	return glc::m_bWindowed;			}

void	g2_SetWindowStyle(DWORD dSty)	{	glc::m_dWinStyle	= dSty;		}
void	g2_SetStateShow(int _bShow)		{	glc::m_bState		= _bShow;	}
void	g2_SetCursorShow(int _bShow)
{
	glc::m_bCursor		= _bShow;
	ShowCursor(glc::m_bCursor);
}

void	g2_SetClearColor(DWORD dC)		{	glc::m_dColor = dC;				}
DWORD	g2_GetClearColor()				{	return glc::m_dColor;			}

void g2_SetWindowTitle(const char *format, ...)
{
	va_list ap;
	std::string s="";
	if(!format)
		return;
	va_start(ap, format);
	int r = vsnprintf(nullptr, 0, format, ap);
	if(0<r)
	{
		s.resize(r);
		r = vsnprintf(&s[0], r+1, format, ap);
	}
	va_end(ap);
	SetWindowTextA(glc::m_hWnd, s.c_str());
}

int g2_SetWindowIcon(long ID)
{
	glc::m_hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID));
	if(glc::m_hWnd && glc::m_hIcon)
	{
		SendMessage(glc::m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)glc::m_hIcon);
		SendMessage(glc::m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)glc::m_hIcon);
	}
	return 0;
}

int g2_Draw2D(const int _nID
			, const RECT* pRct
			, const VEC2* pTrn
			, const VEC2* pScl
			, const VEC2* pRot
			, const float fRad
			, const DWORD dC
			, const int	bMono
			)
{
	int	hr=-1;

	glc::ILcTexture* pTex = glc::LcDev_TextureFind(_nID);
	if(!pTex)
		return 0;


	hr = glc::m_pLcSprite->Begin();
	hr = glc::m_pLcSprite->DrawEx(pTex, pRct, pScl, pTrn, pRot, fRad, dC, bMono);
	glc::m_pLcSprite->End();

	return hr;
}

int g2_DrawAlphaOption(int nAlphaMethod)
{
	if(!glc::m_pLcSprite)
		return -1;

	glc::m_pLcSprite->AlphaOption(nAlphaMethod);
	return 0;
}

int FrameMove2D()
{
	glc::m_timeElapsed = duration_cast<milliseconds>(steady_clock::now() - glc::m_timeBgn).count();
	if(glc::m_pInput)
		glc::m_pInput->FrameMove();
	return glc::g2_FrameMove2D();
}

int Render3D()
{
	glc::m_pd3dDevice->Clear( 0
						, NULL
						, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
						, glc::m_dColor
						, 1.0f, 0 );

	if( FAILED( glc::m_pd3dDevice->BeginScene() ) )
		return -1;

	if( FAILED( glc::g2_Render2D() ) )
	{
		// error check
		int c;
		c = 100;
	};

	glc::m_pd3dDevice->EndScene();

	auto hr = glc::m_pd3dDevice->Present( 0, 0, 0, 0);
	if(D3DERR_DEVICELOST == hr)
		glc::m_bDeviceLost = true;

	return S_OK;
}

LRESULT CALLBACK WndProc(HWND hWnd,unsigned int uMsg,WPARAM wParam,LPARAM lParam)
{
	WPARAM wHi = HIWORD(wParam);
	WPARAM wLo = LOWORD(wParam);


	if(glc::m_pInput)
		glc::m_pInput->MsgProc(hWnd, uMsg, wParam, lParam);

	switch(uMsg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

static int CreateD3D9()
{
	glc::m_d3dppWin.Windowed					= TRUE;
	glc::m_d3dppWin.hDeviceWindow				= glc::m_hWnd;
	glc::m_d3dppWin.SwapEffect				= D3DSWAPEFFECT_DISCARD;
	glc::m_d3dppWin.BackBufferFormat			= D3DFMT_X8R8G8B8;
	glc::m_d3dppWin.BackBufferCount			= 1;
	glc::m_d3dppWin.BackBufferWidth			= UINT(glc::m_scnSize.cx * glc::m_scnScale);
	glc::m_d3dppWin.BackBufferHeight			= UINT(glc::m_scnSize.cy * glc::m_scnScale);
	glc::m_d3dppWin.EnableAutoDepthStencil	= TRUE;
	glc::m_d3dppWin.AutoDepthStencilFormat	= D3DFMT_D24S8;
	glc::m_d3dppFull.FullScreen_RefreshRateInHz= 0;
	glc::m_d3dppWin.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;

	glc::m_d3dppFull							= glc::m_d3dppWin;
	glc::m_d3dppFull.Windowed					= FALSE;

	// D3D생성
	if(NULL == (glc::m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return -1;

	if(FAILED(glc::m_pD3D->CreateDevice(D3DADAPTER_DEFAULT
		, D3DDEVTYPE_HAL
		, glc::m_hWnd
		, D3DCREATE_MIXED_VERTEXPROCESSING
		, &glc::m_d3dppWin
		, &glc::m_pd3dDevice)))
	{
		if(FAILED(glc::m_pD3D->CreateDevice(D3DADAPTER_DEFAULT
			, D3DDEVTYPE_HAL
			, glc::m_hWnd
			, D3DCREATE_SOFTWARE_VERTEXPROCESSING
			, &glc::m_d3dppWin, &glc::m_pd3dDevice)))
		{
			glc::m_pd3dDevice->Release();
			glc::m_pD3D->Release();
			return -1;
		}
	}
	return 0;
}

int g2_InitSdk()
{
	srand((unsigned int)time(NULL));
	g2_SetFrameMove(g2_DefaultFrameMove);
	g2_SetRender(g2_DefaultRender);
	g2_SetKeyboard(g2_DefaultKeyboard);
	g2_SetMouse(g2_DefaultMouse);

	glc::m_hIcon = LoadIcon(NULL,IDI_APPLICATION);
	return 0;
}

int g2_CreateWin(int x, int y, int ScnW, int ScnH, CSTR sName, bool bFull, float scnScale)
{
	strcpy(glc::m_sCls, sName);

	glc::m_scnPos.x = x;
	glc::m_scnPos.y = y;
	glc::m_scnSize.cx = ScnW;
	glc::m_scnSize.cy = ScnH;
	glc::m_scnScale = scnScale;
	g2_ChangeWindow(bFull);

	glc::m_hInst =(HINSTANCE)GetModuleHandle(NULL);

	WNDCLASS wc =
	{
		CS_CLASSDC | CS_DBLCLKS
		, (WNDPROC)WndProc
		, 0
		, 0
		, glc::m_hInst
		, glc::m_hIcon
		, LoadCursor(NULL,IDC_ARROW)
		, (HBRUSH)GetStockObject(WHITE_BRUSH)
		, NULL
		, glc::m_sCls
	};

	RegisterClass(&wc);

	RECT rc;									//Create the application's window
	SetRect( &rc, 0, 0
				, UINT(glc::m_scnSize.cx * glc::m_scnScale)
				, UINT(glc::m_scnSize.cy * glc::m_scnScale) );
	AdjustWindowRect( &rc, glc::m_dWinStyle, FALSE);

	glc::m_hWnd =CreateWindow(
					glc::m_sCls
					, glc::m_sCls
					, glc::m_dWinStyle
					, glc::m_scnPos.x
					, glc::m_scnPos.y
					, (rc.right-rc.left)
					, (rc.bottom-rc.top)
					, NULL, NULL, glc::m_hInst, NULL );

	if(FAILED(CreateD3D9()))
		return -1;

	// DX의 스프라이트는 디바이스가 생성된 후에 만들어야 한다.
	if(FAILED(glc::LcDev_SpriteCreate(&glc::m_pLcSprite, glc::m_pd3dDevice)))
	{
		glc::m_pd3dDevice->Release();
		glc::m_pD3D->Release();
		return -1;
	}

	// DX의 스프라이트는 디바이스가 생성된 후에 만들어야 한다.
	if(FAILED(D3DXCreateSprite(glc::m_pd3dDevice, &glc::m_pd3dSprite)))
	{
		glc::m_pd3dDevice->Release();
		glc::m_pD3D->Release();
		return -1;
	}

	if(FAILED(glc::LcDev_InputCreate(&glc::m_pInput, glc::m_hWnd)))
		return -1;

	D3DXMatrixScaling(&glc::m_tmSprite, scnScale, scnScale, 1.0F);
	glc::LcDev_FontInit(glc::m_pd3dDevice, glc::m_pd3dSprite, &glc::m_tmSprite);
	glc::LcDev_TextureInit(glc::m_pd3dDevice);
	glc::LcDev_SoundInit(glc::m_hWnd);
	glc::LcDev_ModelInit(glc::m_pLcSprite);

	return 0;
}

int g2_ChangeWindow(bool bWindow)
{
	glc::m_bToggledEvent = true;
	glc::m_bToggledWin = bWindow;

	return S_OK;
}

void g2_DestroyWin()
{
	int i=0;

	glc::LcDev_FontDestroy();
	glc::LcDev_TextureDestroy();
	glc::LcDev_SoundDestroy();
	glc::LcDev_ModelDestroy();

	if(glc::m_pInput)		{	delete glc::m_pInput;			glc::m_pInput = {};		}
	if(glc::m_pLcSprite)	{	delete glc::m_pLcSprite;		glc::m_pLcSprite = {};		}
	if(glc::m_pd3dDevice)	{	glc::m_pd3dDevice->Release();	glc::m_pd3dDevice = {};	}
	if(glc::m_pD3D)		{	glc::m_pD3D->Release();		glc::m_pD3D = {};			}
}


int g2_Run()
{
	int hr = S_OK;

	int bGotMsg = FALSE;
	MSG msg = {0};
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while(WM_QUIT != msg.message)
	{
		if(glc::m_bActive)
		{
			bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		}
		else
		{
			bGotMsg = (GetMessage(&msg, NULL, 0U, 0U) != 0);
		}

		if(bGotMsg)
		{
			if(glc::m_hAccel == NULL ||
				glc::m_hWnd == NULL ||
				0 == TranslateAccelerator(glc::m_hWnd, glc::m_hAccel, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			// Device Lost 확인 및 복구
			hr = D3DCheckDevice();
			if(FAILED(hr))
			{
				if(hr == D3DERR_DEVICELOST)
				{
					Sleep(100);
					continue;
				}

				return hr;
			}

			// Window / Fullscreen 전환
			if(glc::m_bToggledEvent)
			{
				hr = D3DToggleScreen();
				if(FAILED(hr))
				{
					if(hr == D3DERR_DEVICELOST)
					{
						glc::m_bDeviceLost = true;
						Sleep(100);
						continue;
					}

					return hr;
				}
			}

			if(FAILED(hr = FrameMove2D()))
			{
				break;
			}

			if(FAILED(hr = Render3D()))
			{
				break;
			}
		}
	}

	return 0;
}


long long g2_TimeGetTime()
{
	auto elapsed = duration_cast<milliseconds>(steady_clock::now() - glc::m_timeBgn).count();
	return elapsed;
}