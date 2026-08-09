
#include "glc2d.h"
#include <stdio.h>

#include "GameHeader.h"
#define TX_NUM		1

int g_GamePhase = GAME_INIT;

int		ImgArrayInit();					// Image Array Init
void	ImgArrayDestroy();				// Image Array Delete
void	ImgArrayShuffle();				// Image Array Shuffle

// Lena
int		nFont1;

VEC3i	m_mousePos;						// mouse position
KEYCODE	m_keyboardOld[GLC_MAX_KEY];		// Old Key
KEYCODE	m_keyboardCur[GLC_MAX_KEY];		// Current Key

int		nSound[2];
int		m_pTx[TX_NUM];		// 배경 텍스처

struct MapBit
{
	int		nIdx;
	RECT	rc;					// Region
};

struct MapImg
{
	int		nIdx;
	VEC2	pos;				// Position
};

int			m_screenW = 640;	// Screen Width
int			m_screenH = 480;	// Screen Height

int			m_nCntRow=3;		// Number of Row
int			m_nCntCol=4;		// Number of column
int			m_nImg=0;			// Index of Image
int			m_iCntSf=4;			// Shuffle Count
int			m_iCntKey=1;		// Keyboard 움직인거..
int			m_iScore=100;		// 점수
int			m_iBlockX;			// block image x size
int			m_iBlockY;			// block image y size

MapBit*		m_pMapBit;			// MapBit m_pMapBit[m_iNumMap];
MapImg**	m_pMapImg;			// correspond the Bitmap...

int			m_iNumMap;			// total block number(실제 1개 더 많음)
int			m_nBlankI;
int			m_nBlankJ;
bool		m_bSuccess=false;


int GameGetScreenWidth(void)
{
	return m_screenW;
}

int GameGetScreenHeight(void)
{
	return m_screenH;
}

int GameInit(void)
{
	m_pTx[0]	= g2_TextureLoad("Texture/img1.bmp", 0xFF000000);
	memset(m_keyboardOld, 0, sizeof(m_keyboardOld));
	memset(m_keyboardCur, 0, sizeof(m_keyboardCur));


	ImgArrayInit();


	nFont1 = g2_FontCreate("Arial", 50, 0);

	//배경 음악
	nSound[0] = g2_SoundLoad("sound/trample.wav");

	//Key board Event
	nSound[1] = g2_SoundLoad("sound/move3.wav");

	g_GamePhase = GAME_PLAY;
//	g2_SoundPlay(nSound[0]);
	return 0;
}


int GameDestroy(void)
{
	ImgArrayDestroy();
	return 0;
}


int GamePlay(void)
{
	m_mousePos = g2_GetMouse();												// 마우스 위치 X,Y,Z
	memcpy(m_keyboardOld, m_keyboardCur, sizeof(m_keyboardCur));				// 키보드 상태를 Old에 저장
	memcpy(m_keyboardCur, g2_GetKeyboard(), sizeof(m_keyboardCur));			// 키보드 상태를 Current에 저장

	int	nCntSuccess=0;
	
	if(m_bSuccess && (0 == m_keyboardOld[VK_HOME] && m_keyboardCur[VK_HOME]) )
	{
		ImgArrayDestroy();

		g_GamePhase = GAME_INIT;
		return 0;
	}

	// cheat
	if(0 == m_keyboardOld[VK_INSERT] && m_keyboardCur[VK_INSERT])
	{
		for(int i=0; i< m_nCntRow; ++i)
		{
			for(int j=0; j< m_nCntCol; ++j)
			{
				m_pMapBit[i*m_nCntCol + j].nIdx =  m_pMapImg[i][j].nIdx;
			}
		}
		return 0;
	}

	// 우측 이동...
	if(0 == m_keyboardOld[VK_RIGHT] && m_keyboardCur[VK_RIGHT] )
	{
		if	(m_nBlankJ<1)
			m_nBlankJ=0;

		else
		{
			m_pMapImg[m_nBlankI][m_nBlankJ].nIdx
				= m_pMapImg[m_nBlankI][m_nBlankJ-1].nIdx;

			m_pMapImg[m_nBlankI][m_nBlankJ-1].nIdx =m_iNumMap ;

			--m_nBlankJ;

			// 점수에 쓸 key 움직임을 증가
			++m_iCntKey;
		}

		g2_SoundStop(nSound[1]);
		g2_SoundReset(nSound[1]);
		g2_SoundPlay(nSound[1]);
	}

	// 좌측 이동...
	if(0 == m_keyboardOld[VK_LEFT] && m_keyboardCur[VK_LEFT] )
	{
		if	(m_nBlankJ>=m_nCntCol-1)
			m_nBlankJ=m_nCntCol-1;

		else
		{
			m_pMapImg[m_nBlankI][m_nBlankJ].nIdx
				= m_pMapImg[m_nBlankI][m_nBlankJ+1].nIdx;

			m_pMapImg[m_nBlankI][m_nBlankJ+1].nIdx =m_iNumMap ;

			++m_nBlankJ;
			++m_iCntKey;
		}

		g2_SoundStop(nSound[1]);
		g2_SoundReset(nSound[1]);
		g2_SoundPlay(nSound[1]);
	}

	// 하측 이동...
	if(0 == m_keyboardOld[VK_DOWN] && m_keyboardCur[VK_DOWN] )
	{
		if	(m_nBlankI<1)
			m_nBlankI=0;

		else
		{
			m_pMapImg[m_nBlankI][m_nBlankJ].nIdx
				= m_pMapImg[m_nBlankI-1][m_nBlankJ].nIdx;

			m_pMapImg[m_nBlankI-1][m_nBlankJ].nIdx =m_iNumMap ;

			--m_nBlankI;
			++m_iCntKey;
		}

		g2_SoundStop(nSound[1]);
		g2_SoundReset(nSound[1]);
		g2_SoundPlay(nSound[1]);
	}

	// 상측 이동...
	if(0 == m_keyboardOld[VK_UP] && m_keyboardCur[VK_UP] )
	{
		if	(m_nBlankI>=m_nCntRow-1)
			m_nBlankI=m_nCntRow-1;

		else
		{
			m_pMapImg[m_nBlankI][m_nBlankJ].nIdx
				= m_pMapImg[m_nBlankI+1][m_nBlankJ].nIdx;

			m_pMapImg[m_nBlankI+1][m_nBlankJ].nIdx =m_iNumMap ;

			++m_nBlankI;
		}

		g2_SoundStop(nSound[1]);
		g2_SoundReset(nSound[1]);
		g2_SoundPlay(nSound[1]);
	}


	for(int i=0; i< m_nCntRow; ++i)
	{
		for(int j=0; j< m_nCntCol; ++j)
		{
			// 인덱스가 같으면...
			if( m_pMapImg[i][j].nIdx == m_pMapBit[i*m_nCntCol + j].nIdx)
				++nCntSuccess;
		}
	}

	// 그림을 다 맞추었는가?
	if (nCntSuccess == (m_iNumMap+1))
	{
		++m_iCntKey;

		// 점수를 만든다.
		m_iScore	= m_iCntKey / m_iNumMap;
		m_bSuccess	= true;


		g2_SoundStop(nSound[0]);
	}

	return 0;
}

int GameRender(void)
{
	int i;
	int j;

	if(false == m_bSuccess)
	{
		for(i=0; i< m_nCntRow; ++i)
		{
			for(j=0; j< m_nCntCol; ++j)
			{
				if( m_pMapImg[i][j].nIdx != m_iNumMap )
				{
					g2_Draw2D(m_pTx[m_nImg]
								, &m_pMapBit[ m_pMapImg[i][j].nIdx ].rc
								, &m_pMapImg[i][j].pos);
				}
			}
		}
	}

	else
	{
		RECT rc={0,0, (LONG)m_screenW, (LONG)m_screenH};
		g2_Draw2D(m_pTx[m_nImg], &rc);

		int c= g2_FontDrawText(nFont1, 200, 300, 500, 340, 0xffFFBB77, "추카추가 !!!");
	}

	return 0;
}



////////////////////////////////////////////////////////////////////////////////
// Desc: Puzzle Data Setting

void ImgArrayDestroy()
{
	int i;

	SAFE_DELETE_ARRAY(m_pMapBit);

	if(m_pMapImg)
	{
		for(i=0; i< m_nCntRow; ++i)
		{
			SAFE_DELETE_ARRAY(m_pMapImg[i]);
		}

		SAFE_DELETE_ARRAY(m_pMapImg);
	}
}


int ImgArrayInit()
{
	int i,j;

	RECT rcImg={0,0,640,480};

	m_bSuccess	= false;
	m_iNumMap = m_nCntCol * m_nCntRow -1;
	m_iBlockX = rcImg.right/m_nCntCol ;
	m_iBlockY = rcImg.bottom/m_nCntRow ;
	m_nBlankI = m_nCntRow-1;
	m_nBlankJ = m_nCntCol-1;


	//기준이 될 이미지 맵을 만든다.
	m_pMapImg = new MapImg*[m_nCntRow];


	//열생성
	for(i=0; i< m_nCntRow; ++i)
		m_pMapImg[i] = new MapImg[m_nCntCol];


	// 이미지를 맵과 동일하게 설정한다.
	m_pMapBit = new MapBit[m_iNumMap+1];

	for(i=0; i<= m_iNumMap; ++i)
	{
		m_pMapBit[i].nIdx   = i;
		m_pMapBit[i].rc.left  = m_iBlockX * (i%m_nCntCol);
		m_pMapBit[i].rc.top   = m_iBlockY * (i/m_nCntCol);
		m_pMapBit[i].rc.right = m_pMapBit[i].rc.left + m_iBlockX -1;
		m_pMapBit[i].rc.bottom= m_pMapBit[i].rc.top  + m_iBlockY -1;
	}

	//이미지의 인덱스를 이미지 맵에 쓴다.
	for(i=0; i< m_nCntRow; ++i)
	{
		for(j=0; j< m_nCntCol; ++j)
		{
			m_pMapImg[i][j].nIdx = m_pMapBit[i*m_nCntCol + j].nIdx;
			m_pMapImg[i][j].pos.x = float(m_iBlockX * j);
			m_pMapImg[i][j].pos.y = float(m_iBlockY * i);
		}
	}

	for(i=0; i<m_iNumMap; ++i)
		for(j=0; j<m_iNumMap; ++j)
			ImgArrayShuffle();

	return 0;
}


void ImgArrayShuffle()
{
	int i, nRand;

	m_iCntSf = 100*m_nCntRow *m_nCntCol + rand()%(10*m_nCntRow *m_nCntCol);

	for(i=0; i<m_iCntSf; ++i)
	{
		nRand = rand()%4;

		// 우측 이동...
		if(nRand==0)
		{
			if	(m_nBlankJ<1)
				m_nBlankJ=0;

			else
			{
				m_pMapImg[m_nBlankI][m_nBlankJ].nIdx
					= m_pMapImg[m_nBlankI][m_nBlankJ-1].nIdx;

				m_pMapImg[m_nBlankI][m_nBlankJ-1].nIdx =m_iNumMap;

				--m_nBlankJ;
			}
		}

		// 좌측 이동...
		else if(nRand==1)
		{
			if	(m_nBlankJ>=m_nCntCol-1)
				m_nBlankJ=m_nCntCol-1;

			else
			{
				m_pMapImg[m_nBlankI][m_nBlankJ].nIdx
					= m_pMapImg[m_nBlankI][m_nBlankJ+1].nIdx;

				m_pMapImg[m_nBlankI][m_nBlankJ+1].nIdx =m_iNumMap;

				++m_nBlankJ;
			}
		}

		// 상측 이동...
		else if(nRand==2)
		{
			if	(m_nBlankI<1)
				m_nBlankI=0;

			else
			{
				m_pMapImg[m_nBlankI][m_nBlankJ].nIdx
					= m_pMapImg[m_nBlankI-1][m_nBlankJ].nIdx;

				m_pMapImg[m_nBlankI-1][m_nBlankJ].nIdx =m_iNumMap;

				--m_nBlankI;
			}
		}

		// 하측 이동...
		else if(nRand==3)
		{
			if	(m_nBlankI>=m_nCntRow-1)
				m_nBlankI=m_nCntRow-1;

			else
			{
				m_pMapImg[m_nBlankI][m_nBlankJ].nIdx
					= m_pMapImg[m_nBlankI+1][m_nBlankJ].nIdx;

				m_pMapImg[m_nBlankI+1][m_nBlankJ].nIdx =m_iNumMap;

				++m_nBlankI;
			}
		}
	}
}

int GameFrameMove(void)
{
	switch(g_GamePhase)
	{
		case GAME_INIT:
			GameInit();
			break;

		case GAME_PLAY:
			GamePlay();
			break;

		//case GAME_END:
		//	GameEnd();
		//	break;
	}

	return 0;
}

