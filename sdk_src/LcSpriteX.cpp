// Implementation of the CLcSpriteX class.
//
////////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <d3d9.h>
#include <cmath>
#include <cstring>

#include "glcInternal.h"
#include "ILcTexture.h"
#include "ILcSpriteX.h"

typedef LPDIRECT3DTEXTURE9	DXTEX;

extern float g2_GetScnScale();

namespace glc
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
    bool m_batch{};
    bool m_linear{};
    int m_filter{-1}, m_blend{-1}, m_mono{-1};
    DWORD m_color{};
    bool m_colorValid{};
    LPDIRECT3DTEXTURE9 m_texture{};

public:
	CLcSpriteX();
	virtual ~CLcSpriteX();

	virtual int		Create(void* =0, void* =0, void* =0, void* =0);
	virtual void	Destroy();

	int		Begin(DWORD=0) override;
	int		End(DWORD=0) override;

	int		OnResetDevice() override;
	int		OnLostDevice() override;

	int		Draw( const void* pTex				// Texture
				, const RECT* pRct				// Draw Region
				, const void* pScl=NULL			// Scaling
				, const void* pTrn=NULL			// Position
				, const DWORD dColor=0xFFFFFFFF	// color
				, const int   bMono=0			// Monotone
				) override;

	int		DrawEx(const void* pTex				// Texture
				, const RECT* pRct				// Draw Region
				, const void* pScl				// Scaling
				, const void* pTrn				// Position
				, const void* pRot				// Rotation Center
				, const float	fAngle				// Rotation Angle(Radian)
				, const DWORD dColor=0xFFFFFFFF	// color
				, const int   bMono=0				// Monotone
				) override;

	void	AlphaOption(int opt=0) override;
    void FilterOption(bool linear=false) override { m_linear = linear; }
protected:
	int		DrawExt(const void* pTex
					, const RECT* pRct
					, const VEC2* pScl
					, const VEC2* pRot
					, const float fRot
					, const VEC2* pTrn
					, const DWORD dColor
					, const int bMono);
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
		"         Out.a  = t0.a * m_Diff.a;             \n"
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
							, UINT(strlen(sShader))
							, nullptr
							, nullptr
							, dFlag
							, nullptr
							, &m_pEft
							, &pError);
    if(FAILED(hr))
    {
        if(pError) OutputDebugStringA(static_cast<const char*>(pError->GetBufferPointer()));
        SAFE_RELEASE(pError);
        return hr;
    }
    SAFE_RELEASE(pError);

	// 정점 선언 생성
	D3DVERTEXELEMENT9 pVertexElement[MAX_FVF_DECL_SIZE]={0};
	D3DXDeclaratorFromFVF(CLcSpriteX::VtxRHWUV1::FVF, pVertexElement);
	return m_pDev->CreateVertexDeclaration(pVertexElement, &m_pFVF);
}

int CLcSpriteX::Begin(DWORD)
{
    if(m_batch) return D3DERR_INVALIDCALL;
    HRESULT hr = m_pEft->SetTechnique("Tech");
    if(FAILED(hr)) return hr;
    UINT passes = 0;
    hr = m_pEft->Begin(&passes, 0);
    if(FAILED(hr)) return hr;
    hr = m_pEft->BeginPass(0);
    if(FAILED(hr)) { m_pEft->End(); return hr; }
    m_batch = true;
    m_filter = m_blend = m_mono = -1;
    m_colorValid = false;
    m_texture = nullptr;
    struct State { D3DRENDERSTATETYPE key; DWORD value; };
    const State states[] = {
        {D3DRS_CULLMODE, D3DCULL_NONE}, {D3DRS_ALPHABLENDENABLE, TRUE},
        {D3DRS_SRCBLEND, D3DBLEND_SRCALPHA}, {D3DRS_BLENDOP, D3DBLENDOP_ADD},
        {D3DRS_ZENABLE, FALSE}, {D3DRS_ZWRITEENABLE, FALSE},
        {D3DRS_ALPHATESTENABLE, FALSE}, {D3DRS_LIGHTING, FALSE},
        {D3DRS_SCISSORTESTENABLE, FALSE}, {D3DRS_STENCILENABLE, FALSE}
    };
    for(const auto& state : states) {
        hr = m_pDev->SetRenderState(state.key, state.value);
        if(FAILED(hr)) { End(); return hr; }
    }
    hr = m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    if(SUCCEEDED(hr)) hr = m_pDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    if(SUCCEEDED(hr)) hr = m_pDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    if(SUCCEEDED(hr)) hr = m_pDev->SetVertexShader(nullptr);
    if(SUCCEEDED(hr)) hr = m_pDev->SetVertexDeclaration(m_pFVF);
    if(FAILED(hr)) End();
    return hr;
}

int CLcSpriteX::End(DWORD)
{
    if(!m_batch) return D3DERR_INVALIDCALL;
    const HRESULT passHr = m_pEft->EndPass();
    const HRESULT hr = m_pEft->End();
    m_batch = false;
    m_texture = nullptr;
    return FAILED(passHr) ? passHr : hr;
}

int CLcSpriteX::OnResetDevice()
{
	LPDIRECT3DSURFACE9	pSfc;
	D3DSURFACE_DESC		desc;

	m_pDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSfc);
	pSfc->GetDesc(&desc);

	m_nScnW = desc.Width;
	m_nScnH = desc.Height;
	pSfc->Release();
	int hr = m_pEft->OnResetDevice();
	return hr;
}

int CLcSpriteX::OnLostDevice()
{
	int hr = m_pEft->OnLostDevice();
	return hr;
}

int CLcSpriteX::Draw(const void* pTex		// Texture
					, const RECT* pRct		// Draw Region
					, const void* pScl		// Scaling
					, const void* pTrn		// Position
					, const DWORD dColor	// color
					, const int   bMono		// Monotone
					)
{
	return DrawExt(pTex, pRct, (VEC2*)pScl, {}, 0, (VEC2*)pTrn, dColor, bMono);
}

int CLcSpriteX::DrawEx(const void* pTex		// Texture
					, const RECT* pRct		// Draw Region
					, const void* pScl		// Scaling
					, const void* pTrn		// Position
					, const void* pRot		// Rotation Center
					, const float fAngle	// Rotation Angle(Radian)
					, const DWORD dColor	// color
					, const int   bMono		// Monotone
					)
{
	return DrawExt(pTex, pRct, (VEC2*)pScl, (VEC2*)pRot, fAngle, (VEC2*)pTrn, dColor, bMono);
}

int CLcSpriteX::DrawExt(const void* pTex
						, const RECT* pRct
						, const VEC2* pScl
						, const VEC2* pRot
						, const float fRot
						, const VEC2* pTrn
						, const DWORD dColor
						, const int bMono)
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
		rc1.right = pLcTx->GetImageSize().x;
		rc1.bottom= pLcTx->GetImageSize().y;
	}

	// 영역 재 지정
	if(rc1.left<0)							rc1.left  = 0;
	if(rc1.right>pLcTx->GetImageSize().x )	rc1.right = pLcTx->GetImageSize().x;
	if(rc1.top<0)							rc1.top   = 0;
	if(rc1.bottom>pLcTx->GetImageSize().y)	rc1.bottom= pLcTx->GetImageSize().y;


	// 영역을 잘못 설정한 경우임
	if(rc1.top>=rc1.bottom || rc1.left>=rc1.right)
		return 0;

	// uv 설정
	uv00.x = float(rc1.left  )/float(pLcTx->GetSurfaceSize().x);
	uv00.y = float(rc1.top   )/float(pLcTx->GetSurfaceSize().y);
	uv01.x = float(rc1.right )/float(pLcTx->GetSurfaceSize().x);
	uv01.y = float(rc1.bottom)/float(pLcTx->GetSurfaceSize().y);

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
	float scnScale = g2_GetScnScale();
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


    if(!m_batch) return D3DERR_INVALIDCALL;
    const int filter = m_linear ? D3DTEXF_LINEAR : D3DTEXF_POINT;
    if(filter != m_filter) {
        m_filter = -1;
        hr = m_pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, filter);
        if(FAILED(hr)) return hr;
        hr = m_pDev->SetSamplerState(0, D3DSAMP_MINFILTER, filter);
        if(FAILED(hr)) return hr;
        m_filter = filter;
    }
    const int blend = m_Alphblend == 1 ? D3DBLEND_DESTALPHA : D3DBLEND_INVSRCALPHA;
    if(blend != m_blend) {
        hr = m_pDev->SetRenderState(D3DRS_DESTBLEND, blend);
        if(FAILED(hr)) return hr;
        m_blend = blend;
    }
    if(pDxTx != m_texture) {
        hr = m_pDev->SetTexture(0, pDxTx);
        if(FAILED(hr)) return hr;
        m_texture = pDxTx;
    }
    bool changed = false;
    if(m_mono != bMono) {
        hr = m_pEft->SetInt("m_bMono", bMono);
        if(FAILED(hr)) return hr;
        m_mono = bMono;
        changed = true;
    }
    if(!m_colorValid || m_color != dColor) {
        hr = m_pEft->SetVector("m_Diff", (D3DXVECTOR4*)&dDiff);
        if(FAILED(hr)) return hr;
        m_color = dColor;
        m_colorValid = true;
        changed = true;
    }
    if(changed) {
        hr = m_pEft->CommitChanges();
        if(FAILED(hr)) { m_mono = -1; m_colorValid = false; return hr; }
    }
    // D3D9 pixel centers: preserve one texel per pixel for bitmap text.
    for(auto& vertex : m_pVtx) { vertex.p.x -= 0.5F; vertex.p.y -= 0.5F; }
    return m_pDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_pVtx, sizeof(VtxRHWUV1));

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

};// namespace glc
////////////////////////////////////////////////////////////////////////////////
