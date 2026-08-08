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

namespace glc2d
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
ILcSpriteX*				m_pSprite		{};		// 2D Sprite

DWORD						m_dColor	= 0xFF006699;
steady_clock::time_point	m_timeBgn	= steady_clock::now();
long long					m_timeElapsed=0;
ILcInput*					m_pInput;

int (*glc2d_FrameMove2D)();
int (*glc2d_Render2D)();

int (*glc2d_Keyboard)(uint8_t* key);
int (*glc2d_Mouse)(int x, int y, int z, int _event);

}// namespace glc2d

int glc2d_DefaultFrameMove()	{	return 0;	}
int glc2d_DefaultRender()		{	return 0;	}
int glc2d_DefaultKeyboard(uint8_t*){	return 0;	}
int glc2d_DefaultMouse(int, int, int, int){	return 0;	}

void glc2d_SetFrameMove(int (*v)() )
{
	glc2d::glc2d_FrameMove2D = v;
}

void glc2d_SetRender(int (*v)() )
{
	glc2d::glc2d_Render2D = v;
}

void glc2d_SetKeyboard(int (*v)(uint8_t*) )
{
	glc2d::glc2d_Keyboard	= v;
}

void glc2d_SetMouse(int (*v)(int, int, int, int) )
{
	glc2d::glc2d_Mouse = v;
}

const KEYCODE* glc2d_GetKeyboard()
{
	return glc2d::m_pInput->GetKeyMap();
}

int	glc2d_GetMouseX()
{
	const float* vcPos = glc2d::m_pInput->GetMousePos();
	return int(vcPos[0]);
}

int	glc2d_GetMouseY()
{
	const float* vcPos = glc2d::m_pInput->GetMousePos();
	return int(vcPos[1]);
}

int glc2d_GetMouseZ()
{
	const float* vcPos = glc2d::m_pInput->GetMousePos();
	return int(vcPos[2]);
}

VEC3i glc2d_GetMouse()
{
	const float* vcPos = glc2d::m_pInput->GetMousePos();
	return VEC3i{int(vcPos[0]), int(vcPos[1]), int(vcPos[2])};
}

int glc2d_GetMouseEvent(int nMouse)
{
	int	nState = glc2d::m_pInput->BtnState(nMouse);
	return nState;
}

DWORD	glc2d_GetWindowStyle()	{	return glc2d::m_dWinStyle;			}
HWND	glc2d_GetHwnd()			{	return glc2d::m_hWnd;				}
int		glc2d_GetScnW()			{	return glc2d::m_scnSize.cx;			}
int		glc2d_GetScnH()			{	return glc2d::m_scnSize.cy;			}
float	glc2d_GetScnScale()		{	return glc2d::m_scnScale;			}
bool	glc2d_GetWindowMode()	{	return glc2d::m_bWindowed;			}

void	glc2d_SetWindowStyle(DWORD dSty)	{	glc2d::m_dWinStyle	= dSty;		}
void	glc2d_SetStateShow(int _bShow)		{	glc2d::m_bState		= _bShow;	}
void	glc2d_SetCursorShow(int _bShow)
{
	glc2d::m_bCursor		= _bShow;
	ShowCursor(glc2d::m_bCursor);
}

void	glc2d_SetClearColor(DWORD dC)		{	glc2d::m_dColor = dC;				}
DWORD	glc2d_GetClearColor()				{	return glc2d::m_dColor;			}

void glc2d_SetWindowTitle(const char *format, ...)
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
	SetWindowTextA(glc2d::m_hWnd, s.c_str());
}

int glc2d_SetWindowIcon(long ID)
{
	glc2d::m_hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID));
	if(glc2d::m_hWnd && glc2d::m_hIcon)
	{
		SendMessage(glc2d::m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)glc2d::m_hIcon);
		SendMessage(glc2d::m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)glc2d::m_hIcon);
	}
	return 0;
}

int glc2d_Draw2D(int _nID
				  , RECT* pRct
				  , VEC2* pTrn
				  , VEC2* pScl
				  , VEC2* pRot
				  , float fRad
				  , DWORD dC
				  , int	bMono
				  )
{
	int	hr=-1;

	glc2d::ILcTexture* pTex = glc2d::LcDev_TextureFind(_nID);
	if(!pTex)
		return 0;


	hr = glc2d::m_pSprite->Begin();
	hr = glc2d::m_pSprite->DrawEx(pTex, pRct, pScl, pTrn, pRot, fRad, dC, bMono);
	glc2d::m_pSprite->End();

	return hr;
}

int glc2d_DrawAlphaOption(int nAlphaMethod)
{
	if(!glc2d::m_pSprite)
		return -1;

	glc2d::m_pSprite->AlphaOption(nAlphaMethod);
	return 0;
}

int FrameMove2D()
{
	glc2d::m_timeElapsed = duration_cast<milliseconds>(steady_clock::now() - glc2d::m_timeBgn).count();
	if(glc2d::m_pInput)
		glc2d::m_pInput->FrameMove();
	return glc2d::glc2d_FrameMove2D();
}

int Render3D()
{
	glc2d::m_pd3dDevice->Clear( 0
						, NULL
						, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
						, glc2d::m_dColor
						, 1.0f, 0 );

	if( FAILED( glc2d::m_pd3dDevice->BeginScene() ) )
		return -1;

	if( FAILED( glc2d::glc2d_Render2D() ) )
	{
		// error check
		int c;
		c = 100;
	};

	glc2d::m_pd3dDevice->EndScene();

	auto hr = glc2d::m_pd3dDevice->Present( 0, 0, 0, 0);
	if(D3DERR_DEVICELOST == hr)
		glc2d::m_bDeviceLost = true;

	return S_OK;
}

LRESULT CALLBACK WndProc(HWND hWnd,unsigned int uMsg,WPARAM wParam,LPARAM lParam)
{
	WPARAM wHi = HIWORD(wParam);
	WPARAM wLo = LOWORD(wParam);


	if(glc2d::m_pInput)
		glc2d::m_pInput->MsgProc(hWnd, uMsg, wParam, lParam);

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
	glc2d::m_d3dppWin.Windowed					= TRUE;
	glc2d::m_d3dppWin.hDeviceWindow				= glc2d::m_hWnd;
	glc2d::m_d3dppWin.SwapEffect				= D3DSWAPEFFECT_DISCARD;
	glc2d::m_d3dppWin.BackBufferFormat			= D3DFMT_X8R8G8B8;
	glc2d::m_d3dppWin.BackBufferCount			= 1;
	glc2d::m_d3dppWin.BackBufferWidth			= UINT(glc2d::m_scnSize.cx * glc2d::m_scnScale);
	glc2d::m_d3dppWin.BackBufferHeight			= UINT(glc2d::m_scnSize.cy * glc2d::m_scnScale);
	glc2d::m_d3dppWin.EnableAutoDepthStencil	= TRUE;
	glc2d::m_d3dppWin.AutoDepthStencilFormat	= D3DFMT_D24S8;
	glc2d::m_d3dppFull.FullScreen_RefreshRateInHz= 0;
	glc2d::m_d3dppWin.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;

	glc2d::m_d3dppFull							= glc2d::m_d3dppWin;
	glc2d::m_d3dppFull.Windowed					= FALSE;

	// D3D생성
	if(NULL == (glc2d::m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return -1;

	if(FAILED(glc2d::m_pD3D->CreateDevice(D3DADAPTER_DEFAULT
		, D3DDEVTYPE_HAL
		, glc2d::m_hWnd
		, D3DCREATE_MIXED_VERTEXPROCESSING
		, &glc2d::m_d3dppWin
		, &glc2d::m_pd3dDevice)))
	{
		if(FAILED(glc2d::m_pD3D->CreateDevice(D3DADAPTER_DEFAULT
			, D3DDEVTYPE_HAL
			, glc2d::m_hWnd
			, D3DCREATE_SOFTWARE_VERTEXPROCESSING
			, &glc2d::m_d3dppWin, &glc2d::m_pd3dDevice)))
		{
			glc2d::m_pd3dDevice->Release();
			glc2d::m_pD3D->Release();
			return -1;
		}
	}
	return 0;
}

int glc2d_InitSdk()
{
	srand((unsigned int)time(NULL));
	glc2d_SetFrameMove(glc2d_DefaultFrameMove);
	glc2d_SetRender(glc2d_DefaultRender);
	glc2d_SetKeyboard(glc2d_DefaultKeyboard);
	glc2d_SetMouse(glc2d_DefaultMouse);

	glc2d::m_hIcon = LoadIcon(NULL,IDI_APPLICATION);
	return 0;
}

int glc2d_CreateWin(int x, int y, int ScnW, int ScnH, CSTR sName, bool bFull, float scnScale)
{
	strcpy(glc2d::m_sCls, sName);

	glc2d::m_scnPos.x = x;
	glc2d::m_scnPos.y = y;
	glc2d::m_scnSize.cx = ScnW;
	glc2d::m_scnSize.cy = ScnH;
	glc2d::m_scnScale = scnScale;
	glc2d_ChangeWindow(bFull);

	glc2d::m_hInst =(HINSTANCE)GetModuleHandle(NULL);

	WNDCLASS wc =
	{
		CS_CLASSDC | CS_DBLCLKS
		, (WNDPROC)WndProc
		, 0
		, 0
		, glc2d::m_hInst
		, glc2d::m_hIcon
		, LoadCursor(NULL,IDC_ARROW)
		, (HBRUSH)GetStockObject(WHITE_BRUSH)
		, NULL
		, glc2d::m_sCls
	};

	RegisterClass(&wc);

	RECT rc;									//Create the application's window
	SetRect( &rc, 0, 0
				, UINT(glc2d::m_scnSize.cx * glc2d::m_scnScale)
				, UINT(glc2d::m_scnSize.cy * glc2d::m_scnScale) );
	AdjustWindowRect( &rc, glc2d::m_dWinStyle, FALSE);

	glc2d::m_hWnd =CreateWindow(
					glc2d::m_sCls
					, glc2d::m_sCls
					, glc2d::m_dWinStyle
					, glc2d::m_scnPos.x
					, glc2d::m_scnPos.y
					, (rc.right-rc.left)
					, (rc.bottom-rc.top)
					, NULL, NULL, glc2d::m_hInst, NULL );

	if(FAILED(CreateD3D9()))
		return -1;

	// DX의 스프라이트는 디바이스가 생성된 후에 만들어야 한다.
	if(FAILED(glc2d::LcDev_SpriteCreate(&glc2d::m_pSprite, glc2d::m_pd3dDevice)))
	{
		glc2d::m_pd3dDevice->Release();
		glc2d::m_pD3D->Release();
		return -1;
	}

	if(FAILED(glc2d::LcDev_InputCreate(&glc2d::m_pInput, glc2d::m_hWnd)))
		return -1;

	glc2d::LcDev_FontInit(glc2d::m_pd3dDevice);
	glc2d::LcDev_TextureInit(glc2d::m_pd3dDevice);
	glc2d::LcDev_SoundInit(glc2d::m_hWnd);
	glc2d::LcDev_ModelInit(glc2d::m_pSprite);

	return 0;
}

int glc2d_ChangeWindow(bool bWindow)
{
	glc2d::m_bToggledEvent = true;
	glc2d::m_bToggledWin = bWindow;

	return S_OK;
}

void glc2d_DestroyWin()
{
	int i=0;

	glc2d::LcDev_FontDestroy();
	glc2d::LcDev_TextureDestroy();
	glc2d::LcDev_SoundDestroy();
	glc2d::LcDev_ModelDestroy();

	if(glc2d::m_pInput)		{	delete glc2d::m_pInput;			glc2d::m_pInput = {};		}
	if(glc2d::m_pSprite)	{	delete glc2d::m_pSprite;		glc2d::m_pSprite = {};		}
	if(glc2d::m_pd3dDevice)	{	glc2d::m_pd3dDevice->Release();	glc2d::m_pd3dDevice = {};	}
	if(glc2d::m_pD3D)		{	glc2d::m_pD3D->Release();		glc2d::m_pD3D = {};			}
}


int glc2d_Run()
{
	int hr = S_OK;

	int bGotMsg = FALSE;
	MSG msg = {0};
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while(WM_QUIT != msg.message)
	{
		if(glc2d::m_bActive)
		{
			bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		}
		else
		{
			bGotMsg = (GetMessage(&msg, NULL, 0U, 0U) != 0);
		}

		if(bGotMsg)
		{
			if(glc2d::m_hAccel == NULL ||
				glc2d::m_hWnd == NULL ||
				0 == TranslateAccelerator(glc2d::m_hWnd, glc2d::m_hAccel, &msg))
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
			if(glc2d::m_bToggledEvent)
			{
				hr = D3DToggleScreen();
				if(FAILED(hr))
				{
					if(hr == D3DERR_DEVICELOST)
					{
						glc2d::m_bDeviceLost = true;
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


long long glc2d_TimeGetTime()
{
	auto elapsed = duration_cast<milliseconds>(steady_clock::now() - glc2d::m_timeBgn).count();
	return elapsed;
}