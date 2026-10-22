#define SPRITE_BENCHMARK_NO_MAIN
#include "SpriteBenchmark.cpp"
#include "../sdk_src/glcInternal.cpp"
#include "../sdk_src/LcxFont.cpp"
#include "../sdk_src/LcFont.cpp"
static glc::ILcSpriteX* activeSprite{};
namespace glc {
int LcDev_TextureDraw(ILcTexture* tex,const RECT* rect,const VEC2* pos,const VEC2* scale,const VEC2* pivot,float angle,DWORD color,int mono,bool linear,int alpha) {
    activeSprite->FilterOption(linear); activeSprite->AlphaOption(alpha);
    return activeSprite->DrawEx(tex,rect,scale,pos,pivot,angle,color,mono);
}
}
#define REQUIRE(x) do { if(!(x)) { std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x); std::exit(1); } } while(0)
struct CountingSprite : glc::CLcSpriteX {
    std::vector<UINT> batches;
    int OnLostDevice() override {
        auto* buffer=m_vertexBuffer; if(buffer) buffer->AddRef();
        int hr=CLcSpriteX::OnLostDevice();
        if(buffer) REQUIRE(buffer->Release()==0);
        return hr;
    }
    int Flush() override {
        if(!m_vertices.empty()) batches.push_back(UINT(m_vertices.size()/4));
        return CLcSpriteX::Flush();
    }
};
struct Fixture {
    Bench bench;
    CountingSprite* sprite{};
    ID3DXSprite* fontSprite{};
    int legacyFont{};
    Fixture() {
        delete bench.lc;
        sprite=new CountingSprite; bench.lc=sprite; activeSprite=sprite;
        CHECK(sprite->Create(bench.device)); CHECK(sprite->OnResetDevice());
        CHECK(D3DXCreateSprite(bench.device,&fontSprite));
        D3DXMATRIX identity; D3DXMatrixIdentity(&identity);
        CHECK(glc::LcDev_FontInit(bench.device,fontSprite,sprite,&identity));
        legacyFont=g2_FontCreate("Arial",20); REQUIRE(legacyFont>0);
    }
    ~Fixture() { glc::LcDev_FontDestroy(); fontSprite->Release(); activeSprite=nullptr; }
    std::vector<DWORD> render(int scenario,bool immediate) {
        CHECK(bench.baseline->Apply()); bench.drain(); sprite->batches.clear();
        CHECK(bench.device->Clear(0,nullptr,D3DCLEAR_TARGET,0xff102030,1,0));
        CHECK(bench.device->BeginScene()); CHECK(sprite->Begin());
        auto draw=[&](int tex,DWORD color,float x,float y,bool linear=false,int blend=0,int mono=0,float angle=0.f) {
            const VEC2 pos(x,y),scale(5,4),pivot(x+20,y+16);
            sprite->FilterOption(linear); sprite->AlphaOption(blend);
            CHECK(sprite->DrawEx(&bench.textures[tex],nullptr,&scale,&pos,angle?&pivot:nullptr,angle,color,mono));
            if(immediate) CHECK(sprite->Flush());
        };
        switch(scenario) {
        case 0: // Compatible quads with varying positions and rotations form one batch.
            for(int i=0;i<100;++i) draw(0,0xffeeddcc,float(i%15*35),float(i/15*40),false,0,0,float(i%4)*.3f);
            break;
        case 1: // A -> B -> A must stay in order at equal Z, including overlap.
            draw(0,0xc0ffffff,20,20); draw(1,0xc0ffffff,25,25); draw(0,0xc0ffffff,30,30);
            break;
        case 2: // Every supported batch state is a boundary.
            draw(0,0xffffffff,10,10); draw(0,0xffaaffff,20,20);
            draw(0,0xffaaffff,30,30,true); draw(0,0xffaaffff,40,40,true,1);
            draw(0,0xffaaffff,50,50,true,1,1); draw(1,0xffaaffff,60,60,true,1,1);
            break;
        case 3: // Capacity flush and ring wrap; the last overlapping quad stays on top.
            for(int i=0;i<10000;++i) draw(0,0xffffffff,float(i%80*8),float(i/80%60*8));
            draw(0,0xff0000ff,10,10);
            break;
        case 4: // Multi-quad VB draws interleaved with singleton UP draws.
            for(int i=0;i<1500;++i) {
                const DWORD color=(i%7<5)?0xffffaaaa:DWORD(0xff000000|(i*12345&0xffffff));
                draw(i%7==6?1:0,color,float(i%80*8),float(i/80%60*8));
            }
            break;
        case 5: { // A queued texture must stay alive after its owner releases it.
            Texture temporary;
            CHECK(bench.device->CreateTexture(8,8,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&temporary.texture,nullptr));
            D3DLOCKED_RECT lock{}; CHECK(temporary.texture->LockRect(0,&lock,nullptr,0));
            for(int y=0;y<8;++y) for(int x=0;x<8;++x)
                reinterpret_cast<DWORD*>(static_cast<unsigned char*>(lock.pBits)+y*lock.Pitch)[x]=0xffffffff;
            CHECK(temporary.texture->UnlockRect(0));
            const VEC2 pos(10,10),scale(10,10);
            CHECK(sprite->DrawEx(&temporary,nullptr,&scale,&pos,nullptr,0,0xffff0000,0));
            CHECK(sprite->DrawEx(&temporary,nullptr,&scale,&pos,nullptr,0,0xffff0000,0));
            if(immediate) CHECK(sprite->Flush());
            temporary.texture->Release(); temporary.texture=nullptr;
            break;
        }
        case 6: // Legacy D3DXFont draws between queued sprites.
            draw(0,0xff555555,10,10);
            REQUIRE(g2_FontDrawText(legacyFont,VEC2i{15,15},0xffffffff,"Legacy text")>0);
            draw(0,0xff555555,30,15); // Same batch key after D3DX state restoration.
            REQUIRE(g2_FontDrawText(legacyFont,VEC2i{35,20},0xffffcc00,"Front")>0);
            draw(0,0xc0ffffff,50,20);
            break;
        case 7: { // Preserve earlier text when the same font texture is edited mid-frame.
            glc::CLcxFont font; CHECK(font.Create("Arial",25,false));
            CHECK(font.SetText("MMMMM")); REQUIRE(font.Draw(VEC2i{20,20},0xffffffff)>0);
            if(immediate) CHECK(sprite->Flush());
            CHECK(font.SetText("ii")); REQUIRE(font.Draw(VEC2i{20,60},0xffffffff)>0);
            if(immediate) CHECK(sprite->Flush());
            CHECK(font.SetText("")); // Destroying font ownership must not invalidate the queued draw.
            break;
        }
        }
        CHECK(sprite->End()); CHECK(bench.device->EndScene()); bench.drain();
        return bench.pixels();
    }
    void resetDevice() {
        CHECK(bench.baseline->Apply());
        bench.baseline->Release(); bench.baseline=nullptr;
        CHECK(sprite->OnLostDevice()); CHECK(bench.dx->OnLostDevice());
        CHECK(fontSprite->OnLostDevice()); CHECK(glc::LcDev_FontOnLostDevice());
        D3DPRESENT_PARAMETERS pp{}; pp.Windowed=TRUE; pp.SwapEffect=D3DSWAPEFFECT_DISCARD;
        pp.hDeviceWindow=GetDesktopWindow(); pp.BackBufferWidth=640; pp.BackBufferHeight=480;
        pp.BackBufferFormat=D3DFMT_X8R8G8B8; pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
        bench.fence->Release(); bench.fence=nullptr;
        bench.readback->Release(); bench.readback=nullptr;
        CHECK(bench.device->Reset(&pp));
        CHECK(bench.device->CreateQuery(D3DQUERYTYPE_EVENT,&bench.fence));
        CHECK(bench.device->CreateOffscreenPlainSurface(640,480,D3DFMT_X8R8G8B8,D3DPOOL_SYSTEMMEM,&bench.readback,nullptr));
        CHECK(sprite->OnResetDevice()); CHECK(bench.dx->OnResetDevice());
        CHECK(fontSprite->OnResetDevice()); CHECK(glc::LcDev_FontOnResetDevice());
        CHECK(bench.device->SetRenderState(D3DRS_ZENABLE,FALSE));
        CHECK(bench.device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE));
        CHECK(bench.device->CreateStateBlock(D3DSBT_ALL,&bench.baseline)); CHECK(bench.baseline->Capture());
    }
};
int main() {
    Fixture f;
    for(int scenario=0;scenario<8;++scenario) {
        const auto expected=f.render(scenario,true);
        const auto actual=f.render(scenario,false);
        REQUIRE(expected.size()==actual.size());
        for(size_t i=0;i<actual.size();++i) REQUIRE((expected[i]&0xffffff)==(actual[i]&0xffffff));
        if(scenario==0) REQUIRE(f.sprite->batches==std::vector<UINT>{100});
        if(scenario==1) REQUIRE(f.sprite->batches==std::vector<UINT>({1,1,1}));
        if(scenario==2) REQUIRE(f.sprite->batches==std::vector<UINT>({1,1,1,1,1,1}));
        if(scenario==3) REQUIRE(f.sprite->batches==std::vector<UINT>({4096,4096,1808,1}));
        if(scenario==5) REQUIRE((actual[15*640+15]&0xffffff)==0xff0000);
        std::printf("PASS scenario %d: identical pixels, %zu batches\n",scenario,f.sprite->batches.size());
    }
    const auto before=f.render(1,false);
    f.resetDevice();
    REQUIRE(before==f.render(1,false));
    std::puts("PASS: device loss/reset recreates dynamic VB and preserves rendering");
    CHECK(f.sprite->Flush());
    std::puts("PASS: empty Flush outside Begin/End");
}
