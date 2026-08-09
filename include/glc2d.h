/**
 * @file    GaLic (Game Library Collection)
 * @brief   A lightweight 2D Game Library built with DirectX.
 * * Version History:
 * 1. Initial development using DirectX SDK (Summer Update 2003).
 * 2. Updated for compatibility with DirectX SDK (June 2010) and Visual Studio 2008.
 * 3. Current Build Environment: 
 * - IDE: Visual Studio 2022
 * - SDK: Microsoft.DXSDK.D3DX (Version 9.29.952.8)
 */

#pragma warning(disable: 4096)
#pragma warning(disable: 4099)

#pragma once
#ifndef _glc2d_H_
#define _glc2d_H_

#include "glcType.h"
#include <windows.h>

//------------------------------------------------------------------------------------------------------------------------------------------
enum EInput
{
    EINPUT_NONE  = 0,
    EINPUT_DOWN  = 1,
    EINPUT_UP	 = 2,
    EINPUT_PRESS = 3,
    EINPUT_DBCLC = 4,

    MAX_INPUT_KEY = 256,
    MAX_INPUT_BTN = 8,
};

enum EFont2DAlign
{
	FNT_ALIGN_H_L = 0x0000,
	FNT_ALIGN_H_C = 0x0001,
	FNT_ALIGN_H_R = 0x0002,

	FNT_ALIGN_V_T = 0x0000,
	FNT_ALIGN_V_C = 0x0010,
	FNT_ALIGN_V_B = 0x0020
};

//------------------------------------------------------------------------------------------------------------------------------------------
// Init Sdk
int		g2_InitSdk();													// Initialize SDK

// Create window and DirectX
int		g2_CreateWin(int x,int y										// Screen position x, y
						 , int ScnW, int ScnH							// Screen Width, height
						 , CSTR sName, bool bFull=true					// Class name, Full screen mode
						 , float scnScale = 1.0F						// Screen Scale
						 );

void	g2_DestroyWin();												// 윈도우와 DirectX소멸
int		g2_Run();														// 게임 루프
int		g2_ChangeWindow(bool bWindow);									// Change Window Mode: windows: true, full: false

//------------------------------------------------------------------------------------------------------------------------------------------
// Setting the runtime Function pointer
void	g2_SetFrameMove( int (*)() );									// 게임 데이터 update 함수
void	g2_SetRender( int (*)() );										// 게임 데이터 렌더링 함수
void	g2_SetKeyboard( int (*)(uint8_t*) );							// 게임엔진에 키보드 이벤트를 가져오는 함수를 연결해주는 함수
void	g2_SetMouse( int (*)(int x,int y,int z,int _event) );			// 게임엔진에 마우스 이벤트를 가져오는 함수를 연결해주는 함수

//------------------------------------------------------------------------------------------------------------------------------------------
// Keyboard and Mouse
const KEYCODE* g2_GetKeyboard();										// 키보드(0~255)
int		g2_GetMouseX();													// 마우스 위치 X
int		g2_GetMouseY();													// 마우스 위치 Y
int		g2_GetMouseZ();													// 마우스 위치 Z
VEC3i	g2_GetMouse();													// 마우스 위치 X,Y,Z
int		g2_GetMouseEvent(int nMouse);									// 마우스 이벤트 (0: LButton, 1: RButton, 2: MButton)

void	g2_SetWindowStyle(DWORD dSty);
DWORD	g2_GetWindowStyle();
HWND	g2_GetHwnd();													// 윈도우 핸들 얻기
int		g2_GetScnW();													// 화면의 크기를 가져오기
int		g2_GetScnH();													// 화면의 너비를 가져오기
float	g2_GetScnScale();												// 화면의 scale 가져오기. fullmode: 1, window mode: xx
bool	g2_GetWindowMode();												// 화면 모드. fullmode: false, window mode: true

//------------------------------------------------------------------------------------------------------------------------------------------
// setup env
void	g2_SetStateShow(int _bShow);									// State 보여주기
void	g2_SetCursorShow(int _bShow);									// Cursor 보여주기
void	g2_SetClearColor(DWORD dC);										// 배경화면 클리어 색상설정
DWORD	g2_GetClearColor();												// 배령화면 클리어 색상 가져오기
void	g2_SetWindowTitle(const char *format, ...);						// 윈도우 타이틀
int		g2_SetWindowIcon(long ID);										// Change Icon

//------------------------------------------------------------------------------------------------------------------------------------------
// 2D Texture
int		g2_TextureLoad(CSTR sFileName, DWORD dc=0x00FFFFFF);			// 이미지 파일 로딩
int		g2_TextureRelease(int _nKey);									// 이미지 파일 해제
int		g2_TextureWidth(int _nKey);										// 이미지 파일 너비
int		g2_TextureHeight(int _nKey);									// 이미지 파일 높이

int		g2_Draw2D(const int _nKey
				, const RECT* pSrcRect									// 그릴 영역
				, const VEC2* pTranslate=NULL							// 이동 위치
				, const VEC2* pScaling=NULL								// 크기 변환 값
				, const VEC2* pRotation=NULL							// 회전 중심 위치
				, const float fAngle=0									// 회전 각도: Radian
				, const DWORD dC=0xFFFFFFFF								// 이미지 파일 그리기
				, const int	bMono=FALSE									// Monotone
				);

int		g2_DrawAlphaOption(int nAlphaMethod=0);

//------------------------------------------------------------------------------------------------------------------------------------------
// for Font
int		g2_FontCreate(CSTR sName, long iH, long iItalic=0);				// 폰트 객체 생성
int		g2_FontDrawText(const int nIdx									// 문자열 출력
					   ,const long left
					   ,const long top
					   ,const long right
					   ,const long bottom
					   ,const DWORD fontColor
					   ,const char *format, ...);

int g2_Font2DCreate(CSTR sName, long iH, long iItalic);
int g2_Font2DSetString(int nID, CSTR sStr);
int g2_Font2DSetPos(int nID, float x, float y, DWORD dwAlign = FNT_ALIGN_H_L | FNT_ALIGN_V_T);
int g2_Font2DSetThick(int nID, int thickX, int thickY);
int g2_Font2DSetColor(int nID, DWORD frontColor, DWORD backColor);
int g2_Font2DDraw(int nID);
int g2_Font2DRelease(int nID);

//------------------------------------------------------------------------------------------------------------------------------------------
// sound
int		g2_SoundLoad(CSTR sFileName);									// 사운드 로딩
int		g2_SoundRelease(int _nKey);										// 사운드 객체 해제
void	g2_SoundPlay(int _nKey);										// 사운드 실행
void	g2_SoundStop(int _nKey);										// 사운드 멈춤
void	g2_SoundReset(int _nKey);										// 사운드 리셋
bool	g2_SoundIsPlaying(int _nKey);									// 사운드 실행중?

//------------------------------------------------------------------------------------------------------------------------------------------
// etc
long long	g2_TimeGetTime();											// return the time GetTime

#endif
