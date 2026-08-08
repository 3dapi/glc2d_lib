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
//------------------------------------------------------------------------------------------------------------------------------------------
// Init Sdk
int		glc2d_InitSdk();													// Initialize SDK

// Create window and DirectX
int		glc2d_CreateWin(int x,int y											// Screen position x, y
						 , int ScnW, int ScnH								// Screen Width, height
						 , CSTR sName, bool bFull=true						// Class name, Full screen mode
						 , float scnScale = 1.0F							// Screen Scale
						 );

void	glc2d_DestroyWin();													// 윈도우와 DirectX소멸
int		glc2d_Run();														// 게임 루프
int		glc2d_ChangeWindow(bool bWindow);									// Change Window Mode: windows: true, full: false

//------------------------------------------------------------------------------------------------------------------------------------------
// Setting the runtime Function pointer
void	glc2d_SetFrameMove( int (*)() );									// 게임 데이터 update 함수
void	glc2d_SetRender( int (*)() );										// 게임 데이터 렌더링 함수
void	glc2d_SetKeyboard( int (*)(uint8_t*) );								// 게임엔진에 키보드 이벤트를 가져오는 함수를 연결해주는 함수
void	glc2d_SetMouse( int (*)(int x,int y,int z,int _event) );			// 게임엔진에 마우스 이벤트를 가져오는 함수를 연결해주는 함수

//------------------------------------------------------------------------------------------------------------------------------------------
// Keyboard and Mouse
const KEYCODE* glc2d_GetKeyboard();										// 키보드(0~255)
int		glc2d_GetMouseX();													// 마우스 위치 X
int		glc2d_GetMouseY();													// 마우스 위치 Y
int		glc2d_GetMouseZ();													// 마우스 위치 Z
VEC3i	glc2d_GetMouse();													// 마우스 위치 X,Y,Z
int		glc2d_GetMouseEvent(int nMouse);									// 마우스 이벤트 (0: LButton, 1: RButton, 2: MButton)

void	glc2d_SetWindowStyle(DWORD dSty);
DWORD	glc2d_GetWindowStyle();
HWND	glc2d_GetHwnd();													// 윈도우 핸들 얻기
int		glc2d_GetScnW();													// 화면의 크기를 가져오기
int		glc2d_GetScnH();													// 화면의 너비를 가져오기
float	glc2d_GetScnScale();												// 화면의 scale 가져오기. fullmode: 1, window mode: xx
bool	glc2d_GetWindowMode();												// 화면 모드. fullmode: false, window mode: true

//------------------------------------------------------------------------------------------------------------------------------------------
// setup env
void	glc2d_SetStateShow(int _bShow);										// State 보여주기
void	glc2d_SetCursorShow(int _bShow);									// Cursor 보여주기
void	glc2d_SetClearColor(DWORD dC);										// 배경화면 클리어 색상설정
DWORD	glc2d_GetClearColor();												// 배령화면 클리어 색상 가져오기
void	glc2d_SetWindowTitle(const char *format, ...);						// 윈도우 타이틀
int		glc2d_SetWindowIcon(long ID);										// Change Icon

//------------------------------------------------------------------------------------------------------------------------------------------
// 2D Texture
int		glc2d_TextureLoad(CSTR sFileName, DWORD dc=0x00FFFFFF);		// 이미지 파일 로딩
int		glc2d_TextureRelease(int _nKey);									// 이미지 파일 해제
int		glc2d_TextureWidth(int _nKey);										// 이미지 파일 너비
int		glc2d_TextureHeight(int _nKey);										// 이미지 파일 높이

int		glc2d_Draw2D(int _nKey
					, RECT* pSrcRect										// 그릴 영역
					, VEC2* pTranslate=NULL									// 이동 위치
					, VEC2* pScaling=NULL									// 크기 변환 값
					, VEC2* pRotation=NULL									// 회전 중심 위치
					, float fAngle=0										// 회전 각도: Radian
					, DWORD dC=0xFFFFFFFF									// 이미지 파일 그리기
					, int	bMono=FALSE										// Monotone
					);

int		glc2d_DrawAlphaOption(int nAlphaMethod=0);

//------------------------------------------------------------------------------------------------------------------------------------------
// for Font
int		glc2d_FontCreate(CSTR sName, long iH, long iItalic=0);		// 폰트 객체 생성
int		glc2d_FontDrawText(int nIdx											// 문자열 출력
					   , long left
					   , long top
					   , long right
					   , long bottom
					   , DWORD fontColor
					   , const char *format, ...);

					   //------------------------------------------------------------------------------------------------------------------------------------------
// sound
int		glc2d_SoundLoad(CSTR sFileName);								// 사운드 로딩
int		glc2d_SoundRelease(int _nKey);										// 사운드 객체 해제
void	glc2d_SoundPlay(int _nKey);											// 사운드 실행
void	glc2d_SoundStop(int _nKey);											// 사운드 멈춤
void	glc2d_SoundReset(int _nKey);										// 사운드 리셋
BOOL	glc2d_SoundIsPlaying(int _nKey);									// 사운드 실행중?

//------------------------------------------------------------------------------------------------------------------------------------------
// etc
long long	glc2d_TimeGetTime();												// return the time GetTime

#endif
