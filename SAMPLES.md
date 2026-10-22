# Sample projects

The `example` directory contains 12 samples that link the library built from `sdk_src`. Each project now has a project reference so building it in Visual Studio also builds the matching library configuration.

The `app_nuget` directory contains 13 samples that consume the local `glc2d 0.1.0.9` package. This package includes the renderer batching changes. `Ex06_StringOut` now demonstrates the same owned A8 font API in both sample sets.

From PowerShell 7 at the repository root, run:

```powershell
./build-samples.ps1
```

The script locates MSBuild, restores the shared DirectX package from installed caches when available, builds the source libraries, creates/refreshes the local NuGet package, and builds all 25 projects in Win32/x64 Debug/Release (100 sample configurations). It requires Visual Studio with the v143 C++ toolset and a `nuget` command on PATH. Nothing is published to an external feed.

For a quicker local check:

```powershell
./build-samples.ps1 -Configuration Debug -Platform x64
```

The default full build should be used when preparing all package configurations. Detailed logs and `summary.csv` are written under `build/samples`.

NuGet packages are shared in the repository's ignored `packages` directory, as configured by `NuGet.Config`. After the first complete build, individual sample solutions can be opened and built normally. Source samples automatically build `sdk_src`; NuGet samples use the local package built by the script. After library changes, rerun the script to refresh that development package.

`sample-common.props` gives every project separate platform/configuration output and object directories, sets C++17, and uses the sample directory as the debugger working directory so relative Texture/Font/Sound paths resolve correctly.

## Rendering with batching

Existing sample calls to `g2_Draw2D`, model draws and font draws are supported. The library owns the frame's Begin/End and submits queued geometry; samples should not add their own Begin/End or flush after every sprite.

Keep draw calls in background, world/characters, then foreground/UI order. Later 2D calls cover earlier calls. Only consecutive draws with the same texture, color, blend, filter and mono state share a batch. Legacy D3DX text and A8 font updates flush earlier draws internally to preserve their order.
