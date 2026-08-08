# glc2d

glc2d는 Visual C++용 2D 게임 라이브러리입니다.

## 프로젝트 설정

NuGet 패키지를 설치한 후 프로젝트의 시작 `.cpp` 파일에 다음 코드를 추가합니다.

```cpp
#include <glc2d.h>

// link the 2d game library
#if defined(_DEBUG)
    #if defined(_M_X64) // 64-bit 아키텍처
        #pragma comment(lib, "glc2d_x64_debug.lib")
    #elif defined(_M_IX86) // 32-bit 아키텍처
        #pragma comment(lib, "glc2d_win32_debug.lib")
    #endif
#else
    #if defined(_M_X64)
        #pragma comment(lib, "glc2d_x64_release.lib")
    #elif defined(_M_IX86)
        #pragma comment(lib, "glc2d_win32_release.lib")
    #endif
#endif
```


# glc2d Manual

## 1. Start

glc2d 프로그램의 기본 실행 순서는 다음과 같습니다.

```text
glc2d_InitSdk()
        ↓
glc2d_CreateWin()
        ↓
glc2d_Run()
        ↓
glc2d_DestroyWin()
```

가장 단순한 프로그램은 다음과 같이 작성할 수 있습니다.

```cpp
#include "glc2d.h"
#include <stdio.h>

int main()
{
    // SDK 초기화
    glc2d_InitSdk();

    printf("Starting ...\n\n");

    // 윈도우 생성
    glc2d_CreateWin(100, 100, 1024, 600, "My First Game Window");

    // 실행
    glc2d_Run();

    // 윈도우 해제
    glc2d_DestroyWin();

    return 0;
}
```

### 주요 함수

| 함수 | 설명 |
|---|---|
| `glc2d_InitSdk()` | glc2d SDK를 초기화합니다. |
| `glc2d_CreateWin(x, y, width, height, title)` | 지정한 위치와 크기로 윈도우를 생성합니다. |
| `glc2d_Run()` | 메인 실행 루프를 시작합니다. |
| `glc2d_DestroyWin()` | 생성한 윈도우를 해제합니다. |


## 2. Texture

이미지를 화면에 출력하려면 텍스처를 로드하고 Render 함수를 등록합니다.

```cpp
int nTx;

int Render()
{
    VEC2 pos(400, 200);

    glc2d_Draw2D(nTx, NULL, &pos);

    return 0;
}

int main()
{
    glc2d_InitSdk();

    // 배경색 설정
    glc2d_SetClearColor(0xFF336699);

    // 화면 출력 함수 등록
    glc2d_SetRender(Render);

    glc2d_CreateWin(100, 100, 800, 600, "Texture");

    // 텍스처 로드
    nTx = glc2d_TextureLoad("Texture/tst.png");

    glc2d_Run();

    // 텍스처 해제
    glc2d_TextureRelease(nTx);

    glc2d_DestroyWin();

    return 0;
}
```

### 텍스처 로드

```cpp
int nTx = glc2d_TextureLoad("Texture/tst.png");
```

`glc2d_TextureLoad()`는 로드한 텍스처의 인덱스를 반환합니다. 이후 그 값을 `glc2d_Draw2D()`에 전달하여 이미지를 출력합니다.

### 이미지 출력

```cpp
VEC2 pos(400, 200);
glc2d_Draw2D(nTx, NULL, &pos);
```

두 번째 인자에 `NULL`을 지정하면 텍스처 전체를 사용합니다.

### 알파 옵션

예제에서는 다음과 같이 알파 옵션을 켜고 이미지를 출력한 뒤 다시 끕니다.

```cpp
glc2d_DrawAlphaOption(1);
glc2d_Draw2D(nTx, NULL, &pos);
glc2d_DrawAlphaOption(0);
```

### 이미지 크기 얻기

```cpp
int width  = glc2d_TextureWidth(nTx);
int height = glc2d_TextureHeight(nTx);
```

텍스처의 폭과 높이는 `glc2d_TextureWidth()`와 `glc2d_TextureHeight()`로 얻을 수 있습니다.


## 3. Mouse

마우스 입력은 FrameMove 함수에서 갱신하는 방식으로 사용할 수 있습니다.

```cpp
int mouseX = 0;
int mouseY = 0;
int mouseZ = 0;

int FrameMove()
{
    mouseX = glc2d_GetMouseX();
    mouseY = glc2d_GetMouseY();
    mouseZ = glc2d_GetMouseZ();

    glc2d_SetWindowTitle("%d %d %d", mouseX, mouseY, mouseZ);

    return 0;
}
```

FrameMove 함수는 프로그램 시작 시 등록합니다.

```cpp
glc2d_SetFrameMove(FrameMove);
```

### 마우스 위치에 이미지 출력

```cpp
int Render()
{
    VEC2 pos(mouseX, mouseY);

    glc2d_Draw2D(nTx, NULL, &pos);

    return 0;
}
```

마우스 좌표를 이미지 위치로 사용하면 이미지가 마우스를 따라 움직이게 만들 수 있습니다.

### 주요 함수

| 함수 | 설명 |
|---|---|
| `glc2d_GetMouseX()` | 현재 마우스 X 좌표를 얻습니다. |
| `glc2d_GetMouseY()` | 현재 마우스 Y 좌표를 얻습니다. |
| `glc2d_GetMouseZ()` | 현재 마우스 Z 값을 얻습니다. |
| `glc2d_GetMouseEvent(index)` | 지정한 마우스 이벤트 상태를 확인합니다. |
| `glc2d_SetWindowTitle(...)` | 윈도우 제목 문자열을 변경합니다. |

오른쪽 마우스 버튼 입력을 확인하는 예는 다음과 같습니다.

```cpp
if (glc2d_GetMouseEvent(1))
{
    // Right Button
}
```


## 4. Keyboard

키보드 상태는 `glc2d_GetKeyboard()`를 통해 얻습니다.

```cpp
const KEYCODE* pKeyboard = glc2d_GetKeyboard();
```

반환된 포인터를 키 코드로 인덱싱하여 현재 키 상태를 확인합니다.

```cpp
for (int i = 9; i < 128; ++i)
{
    if (pKeyboard[i])
    {
        printf("You Pressed %d key!!!\n", i);
    }
}
```

WinAPI 가상 키 코드를 사용할 수도 있습니다.

```cpp
const KEYCODE* pKey = glc2d_GetKeyboard();

if (pKey[VK_SPACE])
{
    glc2d_ChangeWindow();
}
```

또는 방향키를 다음과 같이 검사할 수 있습니다.

```cpp
if (pKey[VK_LEFT])
{
    // Left Key
}

if (pKey[VK_RIGHT])
{
    // Right Key
}
```


## 5. Sprite Animation

스프라이트 시트는 전체 이미지 중 현재 프레임에 해당하는 영역을 `RECT`로 지정하여 출력합니다.

예제에서는 한 프레임의 폭과 전체 프레임 수를 다음과 같이 설정합니다.

```cpp
int   g_AniTex;
int   g_AniImgW;
int   g_AniImgH;
RECT  g_AniRc;

int   g_AniW = 50;
int   g_AniMaxF = 18;
FLOAT g_AniSpeed = 120;
```

### 현재 프레임 계산

```cpp
long long currentTime = glc2d_TimeGetTime();

int frameIndex = int(currentTime / g_AniSpeed);
frameIndex %= g_AniMaxF;
```

현재 시간을 프레임 속도로 나눈 뒤 전체 프레임 수로 나머지 연산을 하여 반복 애니메이션을 만듭니다.

### 출력 영역 계산

```cpp
g_AniRc.left   = (frameIndex + 0) * g_AniW;
g_AniRc.right  = (frameIndex + 1) * g_AniW;
g_AniRc.top    = 0;
g_AniRc.bottom = g_AniImgH;
```

현재 프레임에 해당하는 스프라이트 시트 영역을 계산합니다.

### 애니메이션 출력

```cpp
VEC2 mousePos(g_mouseX, g_mouseY);

glc2d_Draw2D(
    g_AniTex,
    &g_AniRc,
    &mousePos,
    &VEC2(2, 2)
);
```

`glc2d_Draw2D()`에 원본 영역 `RECT`, 출력 위치, 크기 배율을 전달하여 현재 프레임을 화면에 출력합니다.

스프라이트 애니메이션은 별도의 애니메이션 객체를 사용하는 방식이 아니라, 시간에 따라 출력할 `RECT` 영역을 변경하는 방식으로 구성할 수 있습니다.


## 6. 한글 문자열 출력

문자열을 출력하려면 먼저 글꼴을 생성합니다.

```cpp
int nFont1 = glc2d_FontCreate("굴림", 20, 0);
int nFont2 = glc2d_FontCreate("Arial", 25, 1);
int nFont3 = glc2d_FontCreate("궁서", 20, 1);
```

한글 글꼴 이름을 지정하여 한글 문자열을 출력할 수 있습니다.

### 문자열 출력

```cpp
glc2d_FontDrawText(
    nFont2,
    10,
    10,
    500,
    40,
    0xFFFFFF00,
    "출력할 메시지"
);
```

`glc2d_FontDrawText()`는 출력 영역과 색상을 지정할 수 있으며, `printf`와 비슷하게 값을 포함한 문자열도 출력할 수 있습니다.

```cpp
glc2d_FontDrawText(
    nFont3,
    mouseX,
    mouseY,
    mouseX + 500,
    mouseY + 40,
    0xFFFFAAFF,
    "Mouse 위치: %d %d %d ",
    mouseX,
    mouseY,
    mouseZ
);
```

### 기본 사용 형태

```cpp
glc2d_FontDrawText(
    font,
    left,
    top,
    right,
    bottom,
    color,
    format,
    ...
);
```


## 7. WinAPI Resource 활용

WinAPI Resource에 등록한 아이콘을 glc2d 윈도우에 사용할 수 있습니다.

먼저 Resource에서 생성된 헤더를 포함합니다.

```cpp
#include "resource.h"
```

예제에서는 `IDI_MAIN_ICON` Resource ID를 윈도우 아이콘으로 지정합니다.

```cpp
glc2d_InitSdk();

// 아이콘 설정
glc2d_SetWindowIcon(IDI_MAIN_ICON);

glc2d_CreateWin(100, 100, 800, 600, "Resource", false);
```

`glc2d_SetWindowIcon()`은 윈도우 생성 전에 호출합니다.

```cpp
glc2d_SetWindowIcon(IDI_MAIN_ICON);
```

업로드된 예제에서는 WinAPI Resource의 아이콘 사용만 확인할 수 있습니다. `resource.h`와 `IDI_MAIN_ICON`은 Visual Studio 프로젝트의 Resource 설정에서 생성된 값을 사용해야 합니다.

### 윈도우 모드 변경

같은 예제에서는 Space 키로 윈도우 모드를 변경합니다.

```cpp
const KEYCODE* pKey = glc2d_GetKeyboard();

if (pKey[VK_SPACE])
{
    glc2d_ChangeWindow(true/false);
}
```


## 8. Sound

사운드는 파일을 로드하여 얻은 사운드 인덱스로 제어합니다.

```cpp
int nSound1;
int nSound2;

nSound1 = glc2d_SoundLoad("sound/bounce.wav");
nSound2 = glc2d_SoundLoad("sound/move3.wav");
```

예제에서는 WAV 파일을 사용합니다.

### 사운드 재생

```cpp
glc2d_SoundPlay(nSound1);
```

### 재생 상태 확인

```cpp
if (!glc2d_SoundIsPlaying(nSound1))
{
    glc2d_SoundReset(nSound1);
    glc2d_SoundPlay(nSound1);
}
```

이미 재생 중이지 않을 때만 사운드를 처음 위치로 되돌린 후 재생합니다.

### 키보드와 사운드 연결

```cpp
const KEYCODE* pKey = glc2d_GetKeyboard();

if (pKey[VK_LEFT] && !glc2d_SoundIsPlaying(nSound1))
{
    glc2d_SoundReset(nSound1);
    glc2d_SoundPlay(nSound1);
}

if (pKey[VK_RIGHT] && !glc2d_SoundIsPlaying(nSound2))
{
    glc2d_SoundReset(nSound2);
    glc2d_SoundPlay(nSound2);
}
```

### 주요 함수

| 함수 | 설명 |
|---|---|
| `glc2d_SoundLoad(path)` | 사운드 파일을 로드합니다. |
| `glc2d_SoundPlay(index)` | 사운드를 재생합니다. |
| `glc2d_SoundReset(index)` | 사운드 재생 위치를 초기화합니다. |
| `glc2d_SoundIsPlaying(index)` | 현재 사운드가 재생 중인지 확인합니다. |


# 기본 프로그램 구성

여러 기능을 함께 사용할 때는 일반적으로 Render와 FrameMove를 등록하여 프로그램을 구성합니다.

```cpp
int FrameMove()
{
    // 입력 및 게임 상태 갱신

    return 0;
}

int Render()
{
    // 화면 출력

    return 0;
}

int main()
{
    glc2d_InitSdk();

    glc2d_SetClearColor(0xFF006699);

    glc2d_SetRender(Render);
    glc2d_SetFrameMove(FrameMove);

    glc2d_CreateWin(100, 100, 800, 600, "2D Game");

    // Texture, Font, Sound 등의 리소스 로드

    glc2d_Run();

    glc2d_DestroyWin();

    return 0;
}
```

각 함수의 역할은 다음과 같이 구분할 수 있습니다.

```text
FrameMove()
  └─ Mouse
  └─ Keyboard
  └─ Animation Frame
  └─ Game State Update

Render()
  └─ Texture
  └─ Sprite
  └─ String
```
