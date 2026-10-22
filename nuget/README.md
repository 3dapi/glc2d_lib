# glc2d

glc2d는 Windows / Visual C++용 2D 게임 라이브러리입니다. 이미지와 문자열 출력, 키보드·마우스 입력, 스프라이트 시트 애니메이션, WAV·MP3 재생을 제공합니다.

이 문서는 현재 저장소의 `include/glc2d.h`와 구현을 기준으로 작성했습니다.

`0.1.0.9`부터 연속된 같은 상태·색상·텍스처의 2D 그리기를 자동으로 배칭합니다. 호출 순서는 유지되며 나중에 그린 항목이 앞에 표시됩니다. 기존 샘플의 `g2_Draw2D()` 호출은 그대로 사용할 수 있습니다.

## 1. 프로젝트 설정

Visual Studio 2022의 Windows C++ 프로젝트에서 사용합니다. Win32와 x64, Debug와 Release 라이브러리를 제공합니다. 예제는 C++17 기준입니다.

NuGet에서 다음 패키지를 설치합니다.

- `glc2d`
- `Microsoft.DXSDK.D3DX` — 저장소 예제는 `9.29.952.8`을 사용합니다.

현재 `glc2d.nuspec`에는 D3DX 패키지 의존성이 선언되어 있지 않으므로 D3DX 패키지를 별도로 설치합니다. `glc2d.props`는 헤더와 라이브러리의 검색 경로를 추가합니다. 사용할 glc2d 라이브러리는 시작 `.cpp` 파일에서 다음과 같이 선택합니다.

```cpp
#include <glc2d.h>

#if defined(_DEBUG)
    #if defined(_M_X64)
        #pragma comment(lib, "glc2d_x64_debug.lib")
    #elif defined(_M_IX86)
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

런타임 라이브러리는 Debug에서 `/MDd`, Release에서 `/MD`를 사용합니다. 실행 시 필요한 D3DX DLL도 실행 파일의 아키텍처와 구성에 맞아야 합니다. D3DX 패키지의 빌드 설정과 DLL 복사 설정을 유지합니다.

## 2. 기본 실행과 화면 배율

기본 실행 순서는 SDK 초기화 → 콜백 등록 → 창 생성 → 리소스 로드 → 실행 → 해제입니다. 아래 코드는 앞 절의 라이브러리 선택 코드와 함께 사용합니다.

```cpp
#include <glc2d.h>

int FrameMove()
{
    // 입력 확인과 게임 상태 갱신
    return 0;
}

int Render()
{
    // 이미지와 문자열 출력
    return 0;
}

int main()
{
    if (g2_InitSdk() < 0)
        return 1;

    g2_SetFrameMove(FrameMove);
    g2_SetRender(Render);
    g2_SetClearColor(0xFF006699);

    // 논리 화면 800x600, 창 모드, 화면 배율 1.0
    if (g2_CreateWin(100, 100, 800, 600, "2D Game", true, 1.0F) < 0)
        return 1;

    // Texture, Font, Sound 리소스는 창 생성 후 로드합니다.
    g2_Run();
    g2_DestroyWin();
    return 0;
}
```

`g2_CreateWin()`의 선언은 다음과 같습니다.

```cpp
int g2_CreateWin(int x, int y, int ScnW, int ScnH,
                CSTR sName, bool bWindowed = true, float scnScale = 1.0F);
```

여섯 번째 인자 `bWindowed`는 **`true`가 창 모드, `false`가 전체 화면 모드**입니다. `g2_SetWindowMode()`도 같은 방향으로 모드 변경을 요청하며 실제 변경은 게임 루프에서 처리합니다.

```cpp
g2_SetWindowMode(true);   // 창 모드로 변경 요청
g2_SetWindowMode(false);  // 전체 화면 모드로 변경 요청
```

`ScnW`, `ScnH`는 게임의 논리 화면 크기입니다. `scnScale`을 `1.5F`로 지정하면 이미지와 문자열을 1.5배로 출력하며, 창의 클라이언트 크기도 논리 크기의 1.5배로 생성합니다. 마우스 좌표는 이 배율로 나눈 논리 좌표로 반환됩니다. `scnScale`은 라이브러리의 출력 배율이며 Windows 디스플레이 배율을 자동으로 감지하는 옵션은 아닙니다.

| 함수 | 설명 |
|---|---|
| `g2_GetScnW()`, `g2_GetScnH()` | 논리 화면의 폭과 높이 |
| `g2_GetScnScale()` | 생성 시 지정한 화면 배율 |
| `g2_GetWindowMode()` | 창 모드이면 `true`, 전체 화면이면 `false` |
| `g2_GetHwnd()` | 창 핸들 |
| `g2_SetWindowTitle(format, ...)` | 창 제목 변경 |
| `g2_SetClearColor(color)` | 배경색 변경 |

파일 경로는 실행 프로그램의 현재 작업 디렉터리를 기준으로 해석합니다. 예제의 `Texture/`, `sound/` 폴더가 해당 위치에서 접근 가능해야 합니다.

## 3. 이미지 출력

창 생성 후 텍스처를 로드하고, 등록한 `Render()`에서 출력합니다.

```cpp
// 창 생성 후
int texture = g2_TextureLoad("Texture/tst.png");

// Render() 안에서
VEC2 position(400, 200);
g2_Draw2D(texture, nullptr, &position);
```

로드 실패 시 `g2_TextureLoad()`는 `-1`을 반환합니다. 두 번째 인자가 `nullptr`이면 원본 이미지 전체를 사용합니다. 색상 키의 기본값은 `0x00FFFFFF`이며 흰색을 투명 처리합니다. PNG의 알파만 사용하고 색상 키를 끄려면 두 번째 인자로 `0`을 지정합니다.

```cpp
int texture = g2_TextureLoad("Texture/player.png", 0);
int width = g2_TextureWidth(texture);
int height = g2_TextureHeight(texture);
```

이미지 일부와 배율을 지정하려면 다음과 같이 사용합니다. 원본 영역의 `right`, `bottom`은 폭·높이가 아닌 좌표입니다.

```cpp
RECT source{0, 0, 32, 32};
VEC2 position(100, 100);
VEC2 scale(2, 2);
g2_Draw2D(texture, &source, &position, &scale);
```

음수 배율은 이미지를 반전합니다. 출력 시작 위치를 기준으로 양의 폭·높이를 차지합니다.

```cpp
VEC2 flip(-1, 1);
g2_Draw2D(texture, nullptr, &position, &flip);
```

색상은 `0xAARRGGBB`입니다. 기본값 `0xFFFFFFFF`는 원래 색상과 알파를 유지합니다. 마지막 인자의 `bMono`가 참이면 이미지 RGB 대신 지정 색상을 사용하고 이미지 알파를 유지합니다.

```cpp
g2_Draw2D(texture, nullptr, &position, nullptr, nullptr,
          0.0F, 0x80FFFFFF);       // 반투명

g2_Draw2D(texture, nullptr, &position, nullptr, nullptr,
          0.0F, 0xFFFF0000, true); // 빨간색 실루엣
```

회전은 다섯 번째 인자에 회전 중심을 지정하고 여섯 번째 인자에 라디안 각도를 전달합니다. 현재 구현의 회전 중심은 화면 배율이 적용된 백 버퍼 좌표이며, 각도는 내부에서 부호를 반전해 적용합니다. 회전 중심이 `nullptr`이면 각도는 적용하지 않습니다.

`g2_DrawAlphaOption(0)`은 일반적인 원본 알파 블렌딩입니다. `1`은 목적지 알파를 사용하는 별도 블렌딩 방식이며, 알파 사용 여부를 켜고 끄는 옵션이 아닙니다. 설정은 다음 변경까지 유지됩니다.

```cpp
g2_DrawAlphaOption(1);
g2_Draw2D(texture, nullptr, &position);
g2_DrawAlphaOption(0);
```

필요 없어진 텍스처는 `g2_TextureRelease(texture)`로 해제합니다. 남아 있는 텍스처는 `g2_DestroyWin()`에서 해제됩니다.

## 4. 키보드와 마우스

입력 상태는 라이브러리가 갱신한 뒤 `FrameMove()` 콜백에 전달됩니다. 키보드와 마우스 상태는 단순한 참·거짓이 아니라 다음 이벤트 값입니다.

| 값 | 의미 |
|---|---|
| `EINPUT_NONE` | 입력 없음 |
| `EINPUT_DOWN` | 누르기 시작 |
| `EINPUT_UP` | 버튼이나 키를 뗌 |
| `EINPUT_PRESS` | 누른 상태 유지 |
| `EINPUT_DBCLC` | 마우스 더블 클릭 |

`if (keys[VK_SPACE])`처럼 검사하면 키를 뗀 `EINPUT_UP`도 참이 됩니다. 한 번 실행할 동작은 `EINPUT_DOWN`, 지속적인 이동은 `EINPUT_DOWN`과 `EINPUT_PRESS`를 검사합니다.

```cpp
int FrameMove()
{
    const KEYCODE* keys = g2_GetKeyboard();

    if (keys[VK_SPACE] == EINPUT_DOWN)
        g2_SetWindowMode(!g2_GetWindowMode());

    if (keys[VK_LEFT] == EINPUT_DOWN || keys[VK_LEFT] == EINPUT_PRESS)
    {
        // 왼쪽으로 이동
    }

    if (g2_GetMouseEvent(1) == EINPUT_DOWN)
    {
        // 오른쪽 마우스 버튼을 누르기 시작함
    }

    g2_SetWindowTitle("Mouse: %d, %d", g2_GetMouseX(), g2_GetMouseY());
    return 0;
}
```

마우스 버튼 번호는 왼쪽 `0`, 오른쪽 `1`, 가운데 `2`입니다. 좌표는 `g2_GetMouseX()`, `g2_GetMouseY()` 또는 `g2_GetMouse()`로 얻습니다. `g2_GetMouseZ()`는 휠 상태 값입니다.

## 5. 스프라이트 시트 애니메이션

시간에 따라 원본 이미지의 영역을 바꾸면 프레임 애니메이션을 만들 수 있습니다. 다음 예는 폭 50, 높이 64의 프레임 18개가 가로로 배치된 시트입니다.

```cpp
const int frameWidth = 50;
const int frameHeight = 64;
const int frameCount = 18;
const long long frameDurationMs = 120;

int frame = int((g2_TimeGetTime() / frameDurationMs) % frameCount);
RECT source{frame * frameWidth, 0, (frame + 1) * frameWidth, frameHeight};
VEC2 position(100, 100);
VEC2 scale(2, 2);
g2_Draw2D(texture, &source, &position, &scale);
```

`g2_TimeGetTime()`는 라이브러리의 시간 기준점으로부터 경과한 밀리초를 반환합니다. 이미지 출력에는 `D3DXSprite`를 사용하며, 사용자 코드에서 별도의 스프라이트 객체를 생성할 필요는 없습니다.

## 6. 문자열 출력

창 생성 후 시스템에 설치된 글꼴 또는 프로그램에서 등록한 글꼴의 이름으로 글꼴을 만듭니다. 두 번째 인자는 논리 좌표 기준 높이, 세 번째 인자는 이탤릭 여부입니다.

```cpp
int font = g2_FontCreate("굴림", 20);
int italicFont = g2_FontCreate("Arial", 25, 1);
```

폴더에 있는 `.ttf` 파일은 `g2_FontRegisterFile()`로 먼저 등록합니다. 반환값은 등록된 글꼴 수이며 실패하면 `-1`입니다. 같은 절대 경로의 파일을 다시 요청하면 중복 등록하지 않고 기존 등록 결과를 반환합니다. 파일 경로는 UTF-8을 우선 해석하고, 유효한 UTF-8이 아니면 ANSI 코드 페이지로 해석합니다.

```cpp
if (g2_FontRegisterFile("Font/MyGameFont.ttf") > 0)
{
    // 파일명이 아니라 폰트 파일 내부의 글꼴 이름을 사용합니다.
    int gameFont = g2_FontCreate("My Game Font", 24);
}
```

`Font/MyGameFont.ttf`와 `My Game Font`는 예시입니다. 등록은 `g2_FontCreate()` 전에 해야 합니다. 등록된 파일은 현재 프로그램에서만 사용할 수 있으며 시스템에 영구 설치되지 않습니다. 엔진이 `g2_DestroyWin()`에서 모든 폰트 객체를 해제한 뒤 파일 등록도 자동으로 해제하므로 사용자가 등록 해제 함수를 호출할 필요는 없습니다.

`Render()`에서 시작 위치만 지정하면 문자열 전체를 출력할 수 있습니다.

```cpp
g2_FontDrawText(font, {10, 10}, 0xFFFFFF00, "마우스의 오른쪽 버튼 클릭");
g2_FontDrawText(font, {10, 40}, 0xFFFFFFFF,
                "Mouse: %d, %d", g2_GetMouseX(), g2_GetMouseY());
```

중괄호 안의 값이 **2개이면 시작 위치**, **4개이면 출력 영역**입니다. 다른 개수는 `-1`을 반환합니다. 명시적인 `VEC2i`와 `RECT`도 지원합니다.

```cpp
VEC2i position{10, 10};
g2_FontDrawText(font, position, 0xFFFFFFFF, "위치 지정");

RECT region{10, 80, 400, 120};
g2_FontDrawText(font, region, 0xFFFFFFFF, "영역 지정");
g2_FontDrawText(font, {10, 130, 400, 170}, 0xFFFFFFFF, "중괄호 영역");
```

위치 호출은 포맷팅된 문자열을 실제 폰트로 측정하고, 영역에 의한 잘림 없이 출력합니다. 영역 호출은 지정한 영역을 기준으로 출력하며 경계에서 잘릴 수 있습니다. 두 방식 모두 자동 줄바꿈을 사용하지 않으며 `\n`은 줄바꿈으로 처리합니다. 영역 크기에 맞춰 글꼴을 자동으로 확대·축소하지 않습니다.

이탤릭과 폰트별 돌출을 고려하여 출력 영역 오른쪽에 평균 글자 폭 두 개, 아래쪽에 한 줄 높이의 12%를 올림한 여유를 추가합니다. 영역 호출의 실제 오른쪽·아래쪽 경계는 이 여유만큼 늘어납니다. 화면 배율은 문자열과 여유 영역에 함께 적용됩니다.

반환값은 성공 시 문자열 출력 높이의 논리 단위 값입니다. 화면 배율을 곱한 픽셀 높이가 아닙니다. 빈 문자열은 `-2`, 잘못된 글꼴 ID 또는 인자는 `-1`, 실제 출력 실패는 `0` 또는 오류값을 반환할 수 있습니다.

현재 문자열 API는 `DrawTextA`를 사용하는 좁은 문자열 API입니다. 한글 문자열의 인코딩은 실행 환경의 ANSI 코드 페이지에 맞춰야 합니다. UTF-8 바이트를 전달한다고 자동으로 UTF-8로 해석하지는 않습니다.

생성한 글꼴은 `g2_DestroyWin()`에서 해제됩니다.

## 7. WAV와 MP3

창 생성 후 사운드를 로드합니다. WAV는 DirectSound, MP3는 Media Foundation을 사용합니다. 현재 지원하는 확장자는 `.wav`, `.mp3`입니다.

```cpp
int effect = g2_SoundLoad("sound/bounce.wav");
int bgm = g2_SoundLoad("sound/bgm.mp3");

g2_SoundPlay(effect);     // 한 번 재생
g2_SoundPlay(bgm, true);  // 반복 재생
```

재생 상태 확인 함수의 이름은 **`g2_SoundIsPlaying()`**입니다.

```cpp
if (!g2_SoundIsPlaying(effect))
{
    g2_SoundReset(effect);
    g2_SoundPlay(effect);
}
```

게임에서 이 코드를 매 프레임 실행하면 재생이 끝날 때마다 다시 시작합니다. 효과음을 특정 입력에 연결하려면 입력 조건도 함께 검사합니다.

```cpp
const KEYCODE* keys = g2_GetKeyboard();
if (keys[VK_SPACE] == EINPUT_DOWN && !g2_SoundIsPlaying(effect))
{
    g2_SoundReset(effect);
    g2_SoundPlay(effect);
}
```

| 함수 | 설명 |
|---|---|
| `g2_SoundLoad(path)` | 로드한 사운드 ID 반환, 실패 시 `-1` |
| `g2_SoundPlay(id, loop)` | 재생, `loop`의 기본값은 `false` |
| `g2_SoundIsPlaying(id)` | 재생 중인지 확인 |
| `g2_SoundStop(id)` | 재생 중지 |
| `g2_SoundReset(id)` | 재생 위치를 시작점으로 이동 |
| `g2_SoundRelease(id)` | 사운드 해제 |

남아 있는 사운드는 `g2_DestroyWin()`에서 해제됩니다.

## 8. WinAPI 리소스 아이콘

Visual Studio 리소스에 아이콘을 등록하고 생성된 `resource.h`의 ID를 사용합니다. 창 생성 전에 SDK 초기화와 아이콘 설정을 순서대로 호출합니다.

```cpp
#include "resource.h"

g2_InitSdk();
g2_SetWindowIcon(IDI_MAIN_ICON);
g2_CreateWin(100, 100, 800, 600, "Resource", true);
```

`IDI_MAIN_ICON`은 각 프로젝트의 리소스 ID입니다. `resource.h`와 아이콘 리소스가 없는 프로젝트에서는 이 예를 그대로 사용할 수 없습니다.

## 9. 예제와 패키지 구성

저장소의 `example/`에는 기본 실행, 이미지, 마우스, 키보드, 애니메이션, 문자열, 아이콘, 사운드 및 게임 예제가 있습니다. `app_nuget/`에는 패키지를 사용하는 프로젝트가 있습니다.

`nuget/glc2d.nuspec`은 다음 파일을 패키지에 포함합니다.

| 원본 | 패키지 경로 |
|---|---|
| `include/` | `build/native/include/` |
| `lib/` | `build/native/lib/` |
| `nuget/build/native/glc2d.props` | `build/native/glc2d.props` |
| `nuget/README.md` | `README.md` |

패키지를 갱신할 때는 네 구성의 라이브러리와 공개 헤더를 함께 갱신합니다.
