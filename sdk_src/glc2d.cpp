#pragma warning(disable: 4996)

//#pragma comment(lib, "legacy_stdio_definitions.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dxguid.lib")

//#pragma comment(lib, "d3dx9.lib")
//#pragma comment(lib, "DxErr.lib")

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")

#include <chrono>
#include <cmath>
#include <cstring>
#include <string>
#include <windows.h>

#include "glcInternal.h"
#include "ILcInput.h"
#include "ILcFont.h"
#include "ILcTexture.h"
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
DWORD				m_dWinStyle	= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU| WS_VISIBLE;

bool				m_bWindowed		{true};
bool				m_bToggledEvent	{false};
bool				m_bToggledWin	{m_bWindowed};
bool				m_bDeviceLost	{};

DWORD						m_dColor	= 0xFF006699;
steady_clock::time_point	m_timeBgn	= steady_clock::now();
long long					m_timeElapsed=0;
ILcInput*					m_pInput	{};


LPDIRECT3D9				m_pD3D			{};			// D3D
D3DPRESENT_PARAMETERS	m_d3dppWin		{};
D3DPRESENT_PARAMETERS	m_d3dppFull		{};
LPDIRECT3DDEVICE9		m_pd3dDevice	{};		// Device
LPD3DXSPRITE			m_pd3dSprite	{};		// Shared sprite for images and fonts
D3DXMATRIX				m_tmSprite	{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};	

int m_nAlphaMethod{};

int (*g2_FrameMove2D)();
int (*g2_Render2D)();

int (*g2_Keyboard)(uint8_t* key);
int (*g2_Mouse)(int x, int y, int z, int _event);

}// namespace glc

namespace
{
bool spriteBatchActive{};
int spriteFilter = -1;
int spriteDestBlend = -1;
bool spriteTransformValid{};
D3DXMATRIX spriteTransform;

void ResetSpriteCache()
{
    spriteFilter = spriteDestBlend = -1;
    spriteTransformValid = false;
}
}

int glc::LcDev_SpriteTransform(const D3DXMATRIX& transform)
{
    if(!spriteBatchActive)
        return D3DERR_INVALIDCALL;
    if(spriteTransformValid && std::memcmp(&spriteTransform, &transform, sizeof transform) == 0)
        return S_OK;
    const HRESULT hr = glc::m_pd3dSprite->SetTransform(&transform);
    if(SUCCEEDED(hr))
    {
        spriteTransform = transform;
        spriteTransformValid = true;
    }
    return hr;
}

int glc::LcDev_SpritePrepare(SPRITE_FILTER filter, SPRITE_BLEND blend)
{
    if(!spriteBatchActive)
        return D3DERR_INVALIDCALL;
    const int sample = filter == SPRITE_FILTER::Point ? D3DTEXF_POINT : D3DTEXF_LINEAR;
    const int destBlend = blend == SPRITE_BLEND::DestinationAlpha
        ? D3DBLEND_DESTALPHA : D3DBLEND_INVSRCALPHA;
    if(sample == spriteFilter && destBlend == spriteDestBlend)
        return S_OK;

    // Submit queued draws under their old state before changing the device.
    if(spriteFilter != -1)
    {
        const HRESULT hr = glc::m_pd3dSprite->Flush();
        if(FAILED(hr)) return hr;
    }
    auto device = glc::m_pd3dDevice;
    if(destBlend != spriteDestBlend)
    {
        const HRESULT hr = device->SetRenderState(D3DRS_DESTBLEND, destBlend);
        if(FAILED(hr)) return hr;
        spriteDestBlend = destBlend;
    }
    if(sample != spriteFilter)
    {
        // A partial failure must not leave the previous sample mode cached.
        spriteFilter = -1;
        HRESULT hr = device->SetSamplerState(0, D3DSAMP_MAGFILTER, sample);
        if(FAILED(hr)) return hr;
        hr = device->SetSamplerState(0, D3DSAMP_MINFILTER, sample);
        if(FAILED(hr)) return hr;
        spriteFilter = sample;
    }
    return S_OK;
}

int g2_DefaultFrameMove()		{	return 0;	}
int g2_DefaultRender()			{	return 0;	}
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

DWORD	g2_GetWindowStyle()				{	return glc::m_dWinStyle;		}
HWND	g2_GetHwnd()					{	return glc::m_hWnd;				}
int		g2_GetScnW()					{	return glc::m_scnSize.cx;		}
int		g2_GetScnH()					{	return glc::m_scnSize.cy;		}
float	g2_GetScnScale()				{	return glc::m_scnScale;			}
bool	g2_GetWindowMode()				{	return glc::m_bWindowed;		}

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
	if(!format)
		return;
	va_start(ap, format);
	std::string s= glc::FormatString(format, ap);
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
	// Keep the public signature, but never substitute a solid fill for grayscale.
	if(bMono)
		return D3DERR_NOTAVAILABLE;
	return glc::LcDev_TextureDraw(glc::LcDev_TextureFind(_nID), pRct, pTrn, pScl, pRot, fRad, dC, bMono);
}

int glc::LcDev_TextureDraw(ILcTexture* texture, const RECT* pRct,
	const VEC2* pTrn, const VEC2* pScl, const VEC2* pRot,
	float fRad, DWORD dC, int bMono)
{
	if(bMono)
		return D3DERR_NOTAVAILABLE; // No grayscale shader in this backend.
	if(!texture)
		return 0;
	auto sprite = glc::m_pd3dSprite;
	auto device = glc::m_pd3dDevice;
	auto dxTexture = static_cast<LPDIRECT3DTEXTURE9>(texture->GetTexture());
	if(!sprite || !device)
		return D3DERR_INVALIDCALL;
	if(!dxTexture)
		return 0;

	RECT rect = {0, 0, texture->GetImageWidth(), texture->GetImageHeight()};
	if(pRct)
		rect = *pRct;
	if(rect.left < 0)
		rect.left = 0;
	if(rect.top < 0)
		rect.top = 0;
	if(rect.right > texture->GetImageWidth())
		rect.right = texture->GetImageWidth();
	if(rect.bottom > texture->GetImageHeight())
		rect.bottom = texture->GetImageHeight();
	if(rect.left >= rect.right || rect.top >= rect.bottom)
		return 0;

	const VEC2 scale = pScl ? *pScl : VEC2(1, 1);
	const VEC2 position = pTrn ? *pTrn : VEC2(0, 0);
	// Negative scales flip within the positive extent, as in the old renderer.
	const float x = position.x - (scale.x < 0 ? (rect.right - rect.left) * scale.x : 0);
	const float y = position.y - (scale.y < 0 ? (rect.bottom - rect.top) * scale.y : 0);
	const float sx = scale.x * glc::m_tmSprite._11;
	const float sy = scale.y * glc::m_tmSprite._22;
	D3DXMATRIX transform(
		sx, 0, 0, 0,
		0, sy, 0, 0,
		0, 0, 1, 0,
		x * glc::m_tmSprite._11, y * glc::m_tmSprite._22, 0, 1);
	if(pRot && fRad != 0.0F)
	{
		// S * T * screenScale * T(-pivot) * R(-angle) * T(pivot).
		// Keep the existing pivot in back-buffer coordinates and angle sign.
		const float c = std::cos(fRad);
		const float s = std::sin(fRad);
		const float dx = transform._41 - pRot->x;
		const float dy = transform._42 - pRot->y;
		transform._11 = sx * c;
		transform._12 = -sx * s;
		transform._21 = sy * s;
		transform._22 = sy * c;
		transform._41 = dx * c + dy * s + pRot->x;
		transform._42 = -dx * s + dy * c + pRot->y;
	}

	HRESULT hr = glc::LcDev_SpritePrepare(glc::SPRITE_FILTER::Point,
		glc::m_nAlphaMethod == 1 ? glc::SPRITE_BLEND::DestinationAlpha : glc::SPRITE_BLEND::Alpha);
	if(SUCCEEDED(hr))
		hr = glc::LcDev_SpriteTransform(transform);
	if(SUCCEEDED(hr))
		hr = sprite->Draw(dxTexture, &rect, nullptr, nullptr, dC);
	return hr;
}

int g2_DrawAlphaOption(int nAlphaMethod)
{
	if(!glc::m_pd3dSprite)
		return -1;
	glc::m_nAlphaMethod = nAlphaMethod;
	return 0;
}

int FrameMove2D()
{
	if(glc::m_pInput)
		glc::m_pInput->FrameMove();
	return glc::g2_FrameMove2D();
}

int Render3D()
{
	auto device = glc::m_pd3dDevice;
	auto sprite = glc::m_pd3dSprite;
	HRESULT hr = device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, glc::m_dColor, 1.0f, 0);
	if(FAILED(hr))
		return hr;

	hr = device->BeginScene();
	if(FAILED(hr))
		return hr;

	hr = sprite->Begin(D3DXSPRITE_ALPHABLEND);
	if(FAILED(hr))
	{
		device->EndScene();
		return hr;
	}

	ResetSpriteCache(); // Begin/End and device reset can invalidate device state.
	spriteBatchActive = true;
	// Begin configures the common MODULATE/alpha states. Disable mipmapping
	// once per frame; it is independent of image/font and sampling changes.
	hr = device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	if(SUCCEEDED(hr))
		hr = glc::LcDev_SpritePrepare(glc::SPRITE_FILTER::Point, glc::SPRITE_BLEND::Alpha);
	if(SUCCEEDED(hr))
		hr = glc::g2_Render2D();
	const HRESULT endHr = sprite->End();
	spriteBatchActive = false;
	ResetSpriteCache();
	const HRESULT sceneHr = device->EndScene();
	if(FAILED(hr)) return hr;
	if(FAILED(endHr)) return endHr;
	if(FAILED(sceneHr)) return sceneHr;

	hr = device->Present(nullptr, nullptr, nullptr, nullptr);
	if(D3DERR_DEVICELOST == hr)
		glc::m_bDeviceLost = true;
	return hr;
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
	glc::m_d3dppWin.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	glc::m_d3dppWin.BackBufferFormat			= D3DFMT_X8R8G8B8;
	glc::m_d3dppWin.BackBufferCount				= 1;
	glc::m_d3dppWin.BackBufferWidth				= UINT(glc::m_scnSize.cx * glc::m_scnScale);
	glc::m_d3dppWin.BackBufferHeight			= UINT(glc::m_scnSize.cy * glc::m_scnScale);
	glc::m_d3dppWin.EnableAutoDepthStencil		= TRUE;
	glc::m_d3dppWin.AutoDepthStencilFormat		= D3DFMT_D24S8;
	glc::m_d3dppWin.FullScreen_RefreshRateInHz	= 0;
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
	// random seed
	srand((unsigned int)time(NULL));

	auto hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if(FAILED(hr))
	{
		//printf("Error: COM Initialize Failed\n");
		return -1;
	}
	g2_SetFrameMove(g2_DefaultFrameMove);
	g2_SetRender(g2_DefaultRender);
	g2_SetKeyboard(g2_DefaultKeyboard);
	g2_SetMouse(g2_DefaultMouse);

	glc::m_hIcon = LoadIcon(NULL,IDI_APPLICATION);
	return 0;
}

int g2_CreateWin(int x, int y, int ScnW, int ScnH, CSTR sName, bool bWindowed, float scnScale)
{
	strcpy(glc::m_sCls, sName);

	glc::m_scnPos.x = x;
	glc::m_scnPos.y = y;
	glc::m_scnSize.cx = ScnW;
	glc::m_scnSize.cy = ScnH;
	glc::m_scnScale = scnScale;
	g2_SetWindowMode(bWindowed);

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
	if(FAILED(D3DXCreateSprite(glc::m_pd3dDevice, &glc::m_pd3dSprite)))
	{
		glc::m_pd3dDevice->Release();
		glc::m_pD3D->Release();
		return -1;
	}

	if(FAILED(glc::LcDev_InputCreate(&glc::m_pInput, glc::m_hWnd)))
		return -1;

	D3DXMatrixScaling(&glc::m_tmSprite, scnScale, scnScale, 1.0F);
	glc::m_pd3dSprite->SetTransform(&glc::m_tmSprite);
	glc::m_nAlphaMethod = 0;
	glc::LcDev_FontInit(glc::m_pd3dDevice, glc::m_pd3dSprite, &glc::m_tmSprite);
	glc::LcDev_TextureInit(glc::m_pd3dDevice);
	glc::LcDev_SoundInit(glc::m_hWnd);
	glc::LcDev_ModelInit();

	return 0;
}

int g2_SetWindowMode(bool bWindowed)
{
	glc::m_bToggledEvent = true;
	glc::m_bToggledWin = bWindowed;

	return S_OK;
}

void g2_DestroyWin()
{
	int i=0;

	glc::LcDev_FontDestroy();
	glc::LcDev_TextureDestroy();
	glc::LcDev_SoundDestroy();
	glc::LcDev_ModelDestroy();

	if(glc::m_pInput)	{	delete glc::m_pInput;			glc::m_pInput = {};		}
	if(glc::m_pd3dSprite){	glc::m_pd3dSprite->Release();	glc::m_pd3dSprite = {};	}
	if(glc::m_pd3dDevice){	glc::m_pd3dDevice->Release();	glc::m_pd3dDevice = {};	}
	if(glc::m_pD3D)		{	glc::m_pD3D->Release();			glc::m_pD3D = {};		}

	CoUninitialize();
}

int g2_Run()
{
	int hr = S_OK;
	int bGotMsg = FALSE;
	MSG msg = {0};
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while(WM_QUIT != msg.message)
	{
		g2_TimeGetTime();

		// process window messages
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
			continue;
		}

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
		// update game data
		if(FAILED(hr = FrameMove2D()))
		{
			break;
		}
		// rendering game data
		if(FAILED(hr = Render3D()))
		{
			break;
		}
	}

	return 0;
}

long long g2_TimeGetTime()
{
	glc::m_timeElapsed = duration_cast<milliseconds>(steady_clock::now() - glc::m_timeBgn).count();
	return glc::m_timeElapsed;
}
