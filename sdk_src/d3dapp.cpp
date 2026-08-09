#pragma warning(disable: 4996)

#include <chrono>
#include "d3dapp.h"
#include "ILcSpriteX.h"
#include "ILcFont.h"

using std::chrono::steady_clock;

namespace glc2d
{
	extern bool					m_bActive			;
	extern int					m_bCursor			;
	extern int					m_bState			;

	extern HINSTANCE			m_hInst				;
	extern HWND					m_hWnd				;
	extern POINT				m_scnPos			;
	extern SIZE					m_scnSize			;
	extern float				m_scnScale			;
	extern BOOL					m_fullMode			;
	extern DWORD				m_dWinStyle			;

	extern bool						m_bWindowed		;
	extern bool						m_bToggledEvent	;
	extern bool						m_bToggledWin	;
	extern bool						m_bDeviceLost	;
	extern LPDIRECT3D9				m_pD3D			;			// D3D
	extern D3DPRESENT_PARAMETERS	m_d3dppWin		;
	extern D3DPRESENT_PARAMETERS	m_d3dppFull		;
	extern LPDIRECT3DDEVICE9		m_pd3dDevice	;		// Device
	extern ILcSpriteX*				m_pSprite		;		// 2D Sprite

	extern DWORD					m_dColor		;
	extern steady_clock::time_point	m_timeBgn		;
	extern long long 				m_dTimeElapsed	;
}


int D3DRestoreDevice()
{
	int hr = D3DResetDevice(glc2d::m_bWindowed);
	if(FAILED(hr))
	{
		return hr;
	}

	glc2d::m_bDeviceLost = false;
	return S_OK;
}

int D3DCheckDevice()
{
	if(!glc2d::m_bDeviceLost)
	{
		return S_OK;
	}

	int hr = glc2d::m_pd3dDevice->TestCooperativeLevel();
	if(hr == D3DERR_DEVICELOST)
	{
		return D3DERR_DEVICELOST;
	}

	if(hr == D3DERR_DEVICENOTRESET)
	{
		return D3DRestoreDevice();
	}

	if(FAILED(hr))
	{
		return hr;
	}

	glc2d::m_bDeviceLost = false;
	return S_OK;
}

int D3DAdjustWindowForChange(bool bWindowed)
{
	if(bWindowed)
	{
		SetWindowLong(glc2d::m_hWnd, GWL_STYLE, glc2d::m_dWinStyle);
	}
	else
	{
		SetWindowLong(glc2d::m_hWnd, GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE);
	}
	return S_OK;
}

int D3DResetDevice(bool bWindowed)
{
	int hr{};
	D3DPRESENT_PARAMETERS d3dParam = bWindowed? glc2d::m_d3dppWin : glc2d::m_d3dppFull;
	if(glc2d::m_pSprite)
		glc2d::m_pSprite->OnLostDevice();
	hr = glc2d::LcDev_FontOnLostDevice();

	if(FAILED(hr = glc2d::m_pd3dDevice->Reset(&d3dParam)))
		return hr;
	// Sprite Reset
	if(glc2d::m_pSprite)
		glc2d::m_pSprite->OnResetDevice();

	hr = glc2d::LcDev_FontOnResetDevice();

	return S_OK;
}

int D3DToggleScreen()
{
	if(glc2d::m_bWindowed == glc2d::m_bToggledWin)
	{
		glc2d::m_bToggledEvent = false;
		return S_OK;
	}
	D3DAdjustWindowForChange(glc2d::m_bToggledWin);
	int hr = D3DResetDevice(glc2d::m_bToggledWin);
	if(FAILED(hr))
	{
		D3DAdjustWindowForChange(glc2d::m_bWindowed);
		return hr;
	}
	glc2d::m_bWindowed = glc2d::m_bToggledWin;
	glc2d::m_bToggledEvent = false;
	return S_OK;
}
