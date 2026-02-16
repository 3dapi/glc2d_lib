// Implementation of the CLcSpriteX class.
//
////////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <d3d9.h>

#include "ILcTexture.h"
#include "ILcSpriteX.h"

typedef LPDIRECT3DTEXTURE9	DXTEX;

extern float glc2d_GetScnScale();

namespace glc2d
{

class CLcSpriteX : public ILcSpriteX
{
public:
	struct VtxRHWUV1
	{
		VEC2	p;
		float	z;
		float	w;
		VEC2	t0;

		VtxRHWUV1()	: p(0,0), z(0), w(1), t0(0,0){}
		VtxRHWUV1(float X,float Y,float Z
					,float U0,float V0
					,float U1,float V1
					):p(X,Y), z(Z), w(1), t0(U0,V0){}

		enum {FVF = (D3DFVF_XYZRHW|D3DFVF_TEX1),};
	};

public:
	LPDIRECT3DDEVICE9				m_pDev{};			// Device
	LPDIRECT3DVERTEXDECLARATION9	m_pFVF{};
	ID3DXEffect*					m_pEft{};			// ID3DXEffect
	VtxRHWUV1						m_pVtx[4]{};		// Vertex Buffer

	int				m_nScnW			{1024};
	int				m_nScnH			{768};
	int				m_Alphblend		{0};

public:
	CLcSpriteX();
	virtual ~CLcSpriteX();

	virtual int		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();

	virtual	int		Begin(DWORD=0);
	virtual	int		End(DWORD=0);

	virtual	int		OnResetDevice();
	virtual	int		OnLostDevice();

	virtual	int		Draw( void* pTex				// Texture
						, RECT* pRct				// Draw Region
						, void* pScl=NULL			// Scaling
						, void* pTrn=NULL			// Position
						, DWORD dColor=0xFFFFFFFF	// color
						, int   bMono=0				// Monotone
						);

	virtual	int		DrawEx( void* pTex				// Texture
						, RECT* pRct				// Draw Region
						, void* pScl				// Scaling
						, void* pTrn				// Position
						, void* pRot				// Rotation Center
						, float	fAngle				// Rotation Angle(Radian)
						, DWORD dColor=0xFFFFFFFF	// color
						, int   bMono=0				// Monotone
						);

	virtual	void	AlphaOption(int opt=0);
protected:
	int		DrawExt(	void* pTex
						, RECT* pRct
						, VEC2* pScl
						, VEC2* pRot
						, float fRot
						, VEC2* pTrn
						, DWORD dColor
						, int bMono);
};

CLcSpriteX::CLcSpriteX()
{
	m_pVtx[0].t0 = VEC2(0, 0);
	m_pVtx[1].t0 = VEC2(1, 0);
	m_pVtx[2].t0 = VEC2(1, 1);
	m_pVtx[3].t0 = VEC2(0, 1);
}

CLcSpriteX::~CLcSpriteX()
{
	Destroy();
}

void CLcSpriteX::Destroy()
{
	SAFE_RELEASE(	m_pEft		);
	SAFE_RELEASE(	m_pFVF		);
}

int CLcSpriteX::Create(void* p1, void* p2, void* p3, void* p4)
{
	HRESULT	hr=0;
	m_pDev	= (LPDIRECT3DDEVICE9)p1;

	const char sShader[] =
		" int     m_bMono;                              \n"
		" float4  m_Diff;                               \n"
		"                                               \n"
		" sampler smp0 : register(s0);                  \n"
		"                                               \n"
		"                                               \n"
		" float4 PxlProc(float4 Pos0: POSITION          \n"
		"             , float2 Tex0: TEXCOORD0):COLOR0  \n"
		" {                                             \n"
		"     float4  Out= 0;                           \n"
		"     float4  t0 = tex2D(smp0, Tex0);           \n"
		"                                               \n"
		"         Out = t0;                             \n"
		"                                               \n"
		"     Out *= m_Diff;                            \n"
		"     if(0 != m_bMono)                          \n"
		"     {                                         \n"
		"         Out.a *= m_Diff.a;                    \n"
		"         Out.r  = m_Diff.r;                    \n"
		"         Out.g  = m_Diff.g;                    \n"
		"         Out.b  = m_Diff.b;                    \n"
		"     }                                         \n"
		"                                               \n"
		"     return Out;                               \n"
		" }                                             \n"
		"                                               \n"
		" technique Tech                                \n"
		" {                                             \n"
		"    pass P0                                    \n"
		"    {                                          \n"
		"      PixelShader  = compile ps_2_0 PxlProc(); \n"
		"    }                                          \n"
		" };                                            \n"
		;


	LPD3DXBUFFER	pError	= {};
	DWORD			dFlag=0;

	#if defined( _DEBUG ) || defined( DEBUG )
	dFlag |= D3DXSHADER_DEBUG;
	#endif

	// 컴파일
	hr = D3DXCreateEffect(	m_pDev
							, sShader
							, strlen(sShader)-4
							, NULL
							, NULL
							, dFlag
							, NULL
							, &m_pEft
							, &pError);
	if(FAILED(hr))
	{
		MessageBox(GetActiveWindow()
					, (char*)pError->GetBufferPointer()
					, "Error", 0);
		return -1;
	}

	// 정점 선언 생성
	D3DVERTEXELEMENT9 pVertexElement[MAX_FVF_DECL_SIZE]={0};
	D3DXDeclaratorFromFVF(CLcSpriteX::VtxRHWUV1::FVF, pVertexElement);
	m_pDev->CreateVertexDeclaration(pVertexElement, &m_pFVF);

	return 0;
}

int CLcSpriteX::Begin(DWORD dVal){	return 0;	}
int CLcSpriteX::End(DWORD dVal)	{	return 0;	}

int CLcSpriteX::OnResetDevice()
{
	LPDIRECT3DSURFACE9	pSfc;
	D3DSURFACE_DESC		desc;

	m_pDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSfc);
	pSfc->GetDesc(&desc);

	m_nScnW = desc.Width;
	m_nScnH = desc.Height;
	pSfc->Release();

	return m_pEft->OnResetDevice();
}

int CLcSpriteX::OnLostDevice()
{
	return m_pEft->OnLostDevice();
}

int CLcSpriteX::Draw(void* pTex				// Texture
					, RECT* pRct			// Draw Region
					, void* pScl			// Scaling
					, void* pTrn			// Position
					, DWORD dColor			// color
					, int   bMono			// Monotone
					)
{
	return DrawExt(pTex, pRct, (VEC2*)pScl, NULL, 0, (VEC2*)pTrn, dColor, bMono);
}

int CLcSpriteX::DrawEx( void* pTex			// Texture
						, RECT* pRct		// Draw Region
						, void* pScl		// Scaling
						, void* pTrn		// Position
						, void* pRot		// Rotation Center
						, float	fAngle		// Rotation Angle(Radian)
						, DWORD dColor		// color
						, int   bMono		// Monotone
						)
{
	return DrawExt(pTex, pRct, (VEC2*)pScl, (VEC2*)pRot, fAngle, (VEC2*)pTrn, dColor, bMono);
}

int CLcSpriteX::DrawExt(  void* pTex
						, RECT* pRct
						, VEC2* pScl
						, VEC2* pRot
						, float fRot
						, VEC2* pTrn
						, DWORD dColor
						, int bMono)
{
	HRESULT hr=0;

	ILcTexture*	pLcTx= (ILcTexture*)pTex;
	PDTX		pDxTx = {};

	DCOL	dDiff = 0XFFFFFFFF;
	VEC2	vScl(1,1);
	VEC2	vRot(0,0);
	VEC2	vTrn(0,0);

	RECT	rc1	 = {0, 0, 2048, 2048};

	VEC2	uv00(0,0);
	VEC2	uv01(1,1);
	VEC2	uv10(0,0);
	VEC2	uv11(1,1);

	float	rcW1 = 0;
	float	rcH1 = 0;
	float	rcW2 = 0;
	float	rcH2 = 0;

	float	PosL = 0;
	float	PosT = 0;
	float	PosR = 0;
	float	PosB = 0;


	// 텍스처가 없으면 빠져 나감.
	if(!pLcTx || !(pDxTx = (PDTX)pLcTx->GetTexture()) )
		return 0;


	// 1. 입력 값 복사
	dDiff = dColor;
	bMono = bMono;


	// 1.1 이미지 소스 1 영역 복사
	if(pRct)
	{
		rc1.left  = pRct->left  ;
		rc1.right = pRct->right ;
		rc1.top	  = pRct->top	 ;
		rc1.bottom= pRct->bottom;
	}
	else
	{
		rc1.right = pLcTx->GetImageWidth();
		rc1.bottom= pLcTx->GetImageHeight();
	}

	// 영역 재 지정
	if(rc1.left<0)							rc1.left  = 0;
	if(rc1.right>pLcTx->GetImageWidth() )	rc1.right = pLcTx->GetImageWidth();
	if(rc1.top<0)							rc1.top   = 0;
	if(rc1.bottom>pLcTx->GetImageHeight())	rc1.bottom= pLcTx->GetImageHeight();


	// 영역을 잘못 설정한 경우임
	if(rc1.top>=rc1.bottom || rc1.left>=rc1.right)
		return 0;

	// uv 설정
	uv00.x = float(rc1.left  )/float(pLcTx->GetSurfaceWidth());
	uv00.y = float(rc1.top   )/float(pLcTx->GetSurfaceHeight());
	uv01.x = float(rc1.right )/float(pLcTx->GetSurfaceWidth());
	uv01.y = float(rc1.bottom)/float(pLcTx->GetSurfaceHeight());

	if(pScl)	vScl = *pScl;
	if(pRot)	vRot = *pRot;
	if(pTrn)	vTrn = *pTrn;

	rcW1	= float(rc1.right - rc1.left);
	rcH1	= float(rc1.bottom- rc1.top );

	// Scaling 적용
	if(vScl.x>=0.f)
	{
		PosL =  vTrn.x;
		PosR = PosL + rcW1 * vScl.x;
	}
	else
	{
		PosR =  vTrn.x;
		PosL = PosR - rcW1 * vScl.x;
	}


	if(vScl.y>=0.f)
	{
		PosT = vTrn.y;
		PosB = PosT + rcH1 * vScl.y;
	}
	else
	{
		PosB = vTrn.y;
		PosT = PosB - rcH1 * vScl.y;
	}

	// 정점 설정
	float scnScale = glc2d_GetScnScale();
	m_pVtx[0].p = VEC2(PosL, PosT) * scnScale;
	m_pVtx[1].p = VEC2(PosR, PosT) * scnScale;
	m_pVtx[2].p = VEC2(PosR, PosB) * scnScale;
	m_pVtx[3].p = VEC2(PosL, PosB) * scnScale;

	m_pVtx[0].t0= VEC2(uv00.x, uv00.y);
	m_pVtx[1].t0= VEC2(uv01.x, uv00.y);
	m_pVtx[2].t0= VEC2(uv01.x, uv01.y);
	m_pVtx[3].t0= VEC2(uv00.x, uv01.y);


	// 회전 설정
	if(pRot)
	{
		float	fCos = cosf(-fRot);
		float	fSin = sinf(-fRot);

		VEC2	t;

		for(int i=0; i<4; ++i)
		{
			t = m_pVtx[i].p - vRot;

			m_pVtx[i].p.x = t.x * fCos - t.y * fSin + vRot.x;
			m_pVtx[i].p.y = t.x * fSin + t.y * fCos + vRot.y;
		}
	}


	m_pDev->SetSamplerState(0,D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pDev->SetSamplerState(0,D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// 단색일 경우 부드럽게 처리
	if(bMono)
	{
		m_pDev->SetSamplerState(0,D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_pDev->SetSamplerState(0,D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pDev->SetSamplerState(0,D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	}
	else
	{
		m_pDev->SetSamplerState(0,D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		m_pDev->SetSamplerState(0,D3DSAMP_MINFILTER, D3DTEXF_NONE);
		m_pDev->SetSamplerState(0,D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	}

	m_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);


	if(1 == m_Alphblend)
	{
		m_pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
	}
	else
	{
		m_pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}


	m_pDev->SetTexture(0, pDxTx);

	hr = m_pDev->SetVertexDeclaration(m_pFVF);

	hr = m_pEft->SetTechnique("Tech");
	hr = m_pEft->SetInt("m_bMono", bMono);
	hr = m_pEft->SetVector("m_Diff", (D3DXVECTOR4*)&dDiff);

	hr = m_pEft->Begin(NULL, 0);
	hr = m_pEft->BeginPass(0);

		hr = m_pDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_pVtx, sizeof(CLcSpriteX::VtxRHWUV1));

	m_pEft->EndPass();
	m_pEft->End();

	m_pDev->SetTexture(0, NULL);
	m_pDev->SetVertexDeclaration(NULL);
	m_pDev->SetVertexShader(NULL);
	m_pDev->SetPixelShader(NULL);
	m_pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return 0;
}

void CLcSpriteX::AlphaOption(int opt)
{
	m_Alphblend = opt;
}

int LcDev_SpriteCreate(ILcSpriteX** pData
					, void* p1
					, void* p2
					, void* p3
					, void* p4
					)
{
	*pData = {};
	CLcSpriteX*	pObj = new CLcSpriteX;
	if(FAILED(pObj->Create(p1, p2, p3, p4)))
	{
		delete pObj;
		return -1;
	}
	*pData = pObj;
	return 0;
}

};// namespace glc2d
////////////////////////////////////////////////////////////////////////////////
