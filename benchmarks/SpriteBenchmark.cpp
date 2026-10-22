// Compile the current renderer directly so stale .lib files cannot affect the comparison.
#include "../sdk_src/LcSpriteX.cpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
float g2_GetScnScale() { return 1.0f; }
#define CHECK(x) do { HRESULT hr_=(x); if(FAILED(hr_)) { std::fprintf(stderr,"line %d: %s: %08lx\n",__LINE__,#x,(unsigned long)hr_); std::exit(1); } } while(0)
struct Texture final : glc::ILcTexture {
    IDirect3DTexture9* texture{};
    ~Texture() override { if(texture) texture->Release(); }
    int GetID() const override { return 0; }
    VEC2i GetImageSize() const override { return {8,8}; }
    VEC2i GetSurfaceSize() const override { return {8,8}; }
    void* GetTexture() const override { return texture; }
    RECTi GetImageRect() const override { return {0,0,8,8}; }
};
struct Timing { double cpu, completed; };
struct Bench {
    IDirect3D9* d3d{}; IDirect3DDevice9* device{};
    glc::ILcSpriteX* lc{}; ID3DXSprite* dx{};
    IDirect3DQuery9* fence{}; IDirect3DStateBlock9* baseline{};
    IDirect3DSurface9* readback{}; Texture textures[8];
    LARGE_INTEGER frequency{};
    Bench() {
        QueryPerformanceFrequency(&frequency);
        d3d=Direct3DCreate9(D3D_SDK_VERSION); if(!d3d) std::exit(1);
        D3DADAPTER_IDENTIFIER9 adapter{}; CHECK(d3d->GetAdapterIdentifier(0,0,&adapter));
        std::printf("GPU: %s, driver %08lx:%08lx\n",adapter.Description,adapter.DriverVersion.HighPart,adapter.DriverVersion.LowPart);
        D3DPRESENT_PARAMETERS pp{}; pp.Windowed=TRUE; pp.SwapEffect=D3DSWAPEFFECT_DISCARD;
        pp.hDeviceWindow=GetDesktopWindow(); pp.BackBufferWidth=640; pp.BackBufferHeight=480;
        pp.BackBufferFormat=D3DFMT_X8R8G8B8; pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
        CHECK(d3d->CreateDevice(0,D3DDEVTYPE_HAL,pp.hDeviceWindow,D3DCREATE_HARDWARE_VERTEXPROCESSING,&pp,&device));
        CHECK(glc::LcDev_SpriteCreate(&lc,device)); CHECK(lc->OnResetDevice());
        CHECK(D3DXCreateSprite(device,&dx)); CHECK(device->CreateQuery(D3DQUERYTYPE_EVENT,&fence));
        for(int k=0;k<8;++k) {
            CHECK(device->CreateTexture(8,8,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&textures[k].texture,nullptr));
            D3DLOCKED_RECT lock{}; CHECK(textures[k].texture->LockRect(0,&lock,nullptr,0));
            for(int y=0;y<8;++y) for(int x=0;x<8;++x)
                reinterpret_cast<DWORD*>(static_cast<unsigned char*>(lock.pBits)+y*lock.Pitch)[x]=D3DCOLOR_ARGB(128+x*16,240-k*17,210-y*12,170+k*9);
            CHECK(textures[k].texture->UnlockRect(0));
        }
        CHECK(device->SetRenderState(D3DRS_ZENABLE,FALSE));
        CHECK(device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE));
        CHECK(device->CreateStateBlock(D3DSBT_ALL,&baseline)); CHECK(baseline->Capture());
        CHECK(device->CreateOffscreenPlainSurface(640,480,D3DFMT_X8R8G8B8,D3DPOOL_SYSTEMMEM,&readback,nullptr));
    }
    ~Bench() {
        readback->Release(); baseline->Release(); fence->Release(); dx->Release(); delete lc;
        for(auto& t:textures) { t.texture->Release(); t.texture=nullptr; }
        device->Release(); d3d->Release();
    }
    void drain() {
        CHECK(fence->Issue(D3DISSUE_END));
        LARGE_INTEGER start{},now{}; QueryPerformanceCounter(&start);
        HRESULT hr;
        while((hr=fence->GetData(nullptr,0,D3DGETDATA_FLUSH))==S_FALSE) {
            QueryPerformanceCounter(&now); if(double(now.QuadPart-start.QuadPart)/frequency.QuadPart>10) { std::fprintf(stderr,"GPU timeout\n"); std::exit(1); }
            YieldProcessor();
        }
        CHECK(hr);
    }
    Timing frame(bool useDX, int count, int scenario) {
        CHECK(baseline->Apply()); drain();
        LARGE_INTEGER a{},b{},c{}; QueryPerformanceCounter(&a);
        CHECK(device->Clear(0,nullptr,D3DCLEAR_TARGET,0xff102030,1,0));
        CHECK(device->BeginScene());
        if(useDX) {
            CHECK(dx->Begin(D3DXSPRITE_ALPHABLEND));
            CHECK(device->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT));
            CHECK(device->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT));
            CHECK(device->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE));
        } else { lc->FilterOption(false); lc->AlphaOption(0); CHECK(lc->Begin()); }
        CHECK(device->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE));
        for(int i=0;i<count;++i) {
            const int k=scenario==1 ? i%8 : 0;
            const DWORD color=scenario==2 ? D3DCOLOR_ARGB(255,(i*17)%256,(i*31)%256,(i*53)%256) : 0xffccbbaa;
            const float x=float((i%80)*8), y=float((i/80%60)*8);
            if(useDX) { const D3DXVECTOR3 pos(x,y,0); CHECK(dx->Draw(textures[k].texture,nullptr,nullptr,&pos,color)); }
            else { const VEC2 pos(x,y); CHECK(lc->Draw(&textures[k],nullptr,nullptr,&pos,color,0)); }
        }
        if(useDX) CHECK(dx->End()); else CHECK(lc->End());
        CHECK(device->EndScene()); QueryPerformanceCounter(&b);
        drain(); QueryPerformanceCounter(&c);
        return {double(b.QuadPart-a.QuadPart)*1000/frequency.QuadPart,double(c.QuadPart-a.QuadPart)*1000/frequency.QuadPart};
    }
    std::vector<DWORD> pixels() {
        IDirect3DSurface9* surface{}; CHECK(device->GetRenderTarget(0,&surface));
        CHECK(device->GetRenderTargetData(surface,readback)); surface->Release();
        D3DLOCKED_RECT lock{}; CHECK(readback->LockRect(&lock,nullptr,D3DLOCK_READONLY));
        std::vector<DWORD> result(640*480);
        for(int y=0;y<480;++y) std::memcpy(result.data()+y*640,static_cast<unsigned char*>(lock.pBits)+y*lock.Pitch,640*4);
        CHECK(readback->UnlockRect()); return result;
    }
};
static double median(std::vector<double> values) { std::sort(values.begin(),values.end()); return values[values.size()/2]; }
#ifndef SPRITE_BENCHMARK_NO_MAIN
int main() {
    Bench bench;
    const char* scenarios[]={"same_texture","alternating_8_textures","changing_color"};
    for(int scenario=0;scenario<3;++scenario) {
        bench.frame(false,1000,scenario); const auto lc=bench.pixels();
        bench.frame(true,1000,scenario); const auto dx=bench.pixels();
        unsigned differences=0,maxDelta=0;
        for(size_t i=0;i<lc.size();++i) for(int shift=0;shift<24;shift+=8) {
            unsigned delta=unsigned(std::abs(int((lc[i]>>shift)&255)-int((dx[i]>>shift)&255)));
            differences+=delta!=0; maxDelta=std::max(maxDelta,delta);
        }
        std::printf("Image check %s: different RGB channels=%u, max delta=%u\n",scenarios[scenario],differences,maxDelta);
        if(maxDelta>1) return 2;
    }
    std::ofstream csv("benchmarks/sprite-results.csv");
    csv<<"scenario,sprites,renderer,cpu_median_ms,completion_median_ms,samples\n";
    for(int scenario=0;scenario<3;++scenario) for(int count:{100,1000,10000}) {
        for(int warm=0;warm<5;++warm) { bench.frame(false,count,scenario); bench.frame(true,count,scenario); }
        std::vector<double> cpu[2],completed[2];
        for(int sample=0;sample<21;++sample) for(int order=0;order<2;++order) {
            const int renderer=(sample+order)%2; const auto time=bench.frame(renderer!=0,count,scenario);
            cpu[renderer].push_back(time.cpu); completed[renderer].push_back(time.completed);
        }
        for(int renderer=0;renderer<2;++renderer) {
            const auto c=median(cpu[renderer]),g=median(completed[renderer]);
            csv<<scenarios[scenario]<<','<<count<<','<<(renderer?"D3DXSprite":"ILcSpriteX")<<','<<c<<','<<g<<",21\n";
            std::printf("%s %d %s CPU %.4f ms complete %.4f ms\n",scenarios[scenario],count,renderer?"D3DXSprite":"ILcSpriteX",c,g);
        }
        std::fflush(stdout);
    }
}

#endif
