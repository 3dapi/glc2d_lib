// GaLic Game Library Collection for simple 2d Game.
//
// This library constructed with DirectX SDK summer update 2003
//   and modified on DirectX SDK 2010 jun( it's need to compiling).
//
// Build Env: DirectX SDK 2010 jun, Visual studio 2008
//
////////////////////////////////////////////////////////////////////////////////

#pragma warning(disable: 4096)
#pragma warning(disable: 4099)

#pragma once
#ifndef _glc2d_H_
#define _glc2d_H_

#include <stdint.h>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

typedef D3DXVECTOR2							VEC2;
typedef D3DXVECTOR3							VEC3;
typedef D3DXCOLOR							DCLR;

typedef	LPDIRECT3D9							PD3D;
typedef LPDIRECT3DDEVICE9					PDEV;
typedef	LPD3DXSPRITE						PDSP;

typedef LPDIRECT3DTEXTURE9					PDTX;
typedef LPDIRECT3DSURFACE9					PDSF;
typedef	D3DXIMAGE_INFO						DIMG;

int		glc2d_InitSdk();											// Initialize SDK

// Create window and DirectX
int		glc2d_CreateWin(int x,int y									// Screen position x, y
						 , int ScnW, int ScnH						// Screen Width, height
						 , char* sName, int bFull=FALSE				// Class name, Full screen mode
						 , float scnScale = 1.6F					// Screen Scale
						 );

void	glc2d_DestroyWin();											// 윈도우와 DirectX소멸
int		glc2d_ChangeWindow(int bFull=TRUE);							// Change Window Mode
int		glc2d_Run();												// 게임 루프


// Setting the runtime Function pointer
void	glc2d_SetFrameMove( int (*)() );							// 게임 데이터 update 함수
void	glc2d_SetRender( int (*)() );								// 게임 데이터 렌더링 함수
void	glc2d_SetKeyboard( int (*)(uint8_t*) );						// 게임엔진에 키보드 이벤트를 가져오는 함수를 연결해주는 함수
void	glc2d_SetMouse( int (*)(int x,int y,int z,int _event) );	// 게임엔진에 마우스 이벤트를 가져오는 함수를 연결해주는 함수


// Keyboard and Mouse
const uint8_t*	glc2d_GetKeyboard();								// 키보드(0~255)
int		glc2d_GetMouseX();											// 마우스 위치 X
int		glc2d_GetMouseY();											// 마우스 위치 Y
int		glc2d_GetMouseZ();											// 마우스 위치 Z
int		glc2d_GetMouseEvent(int nMouse);							// 마우스 이벤트 (0: LButton, 1: RButton, 2: MButton)

void	glc2d_SetWindowStyle(DWORD dSty);
DWORD	glc2d_GetWindowStyle();
HWND	glc2d_GetHwnd();											// 윈도우 핸들 얻기
int		glc2d_GetScnW();											// 화면의 크기를 가져오기
int		glc2d_GetScnH();											// 화면의 너비를 가져오기
float	glc2d_GetScnScale();										// 화면의 scale 가져오기. fullmode: 1, window mode: xx

void	glc2d_SetStateShow(int _bShow);								// State 보여주기
void	glc2d_SetCursorShow(int _bShow);							// Cursor 보여주기
void	glc2d_SetClearColor(DWORD dC);								// 배경화면 클리어 색상설정
DWORD	glc2d_GetClearColor();										// 배령화면 클리어 색상 가져오기
void	glc2d_SetWindowTitle(const char *format, ...);				// 윈도우 타이틀


// 2D Texture
int		glc2d_TextureLoad(char* sFileName, DWORD dc=0x00FFFFFF);	// 이미지 파일 로딩
int		glc2d_TextureRelease(int _nKey);							// 이미지 파일 해제
int		glc2d_TextureWidth(int _nKey);								// 이미지 파일 너비
int		glc2d_TextureHeight(int _nKey);								// 이미지 파일 높이

int		glc2d_Draw2D(int _nKey
					, RECT* pSrcRect								// 그릴 영역
					, VEC2* pTranslate=NULL							// 이동 위치
					, VEC2* pScaling=NULL							// 크기 변환 값
					, VEC2* pRotation=NULL							// 회전 중심 위치
					, float fAngle=0								// 회전 각도: Radian
					, DWORD dC=0xFFFFFFFF							// 이미지 파일 그리기
					, int	bMono=FALSE								// Monotone
					);


int		glc2d_DrawAlphaOption(int nAlphaMethod=0);


// for Font
int		glc2d_FontCreate(char* sName, long iH, long iItalic=0);	// 폰트 객체 생성
int		glc2d_FontDrawText(int nIdx								// 문자열 출력
					   , long left
					   , long top
					   , long right
					   , long bottom
					   , DWORD fontColor
					   , const char *format, ...);


// sound
int		glc2d_SoundLoad(char* sFileName);					// 사운드 로딩
int		glc2d_SoundRelease(int _nKey);						// 사운드 객체 해제
void	glc2d_SoundPlay(int _nKey);							// 사운드 실행
void	glc2d_SoundStop(int _nKey);							// 사운드 멈춤
void	glc2d_SoundReset(int _nKey);						// 사운드 리셋
BOOL	glc2d_SoundIsPlaying(int _nKey);					// 사운드 실행중?


// etc
DWORD	glc2d_TimeGetTime();								// return the time GetTime

#endif

