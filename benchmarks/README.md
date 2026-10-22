# Sprite rendering and batching checks

The current `ILcSpriteX` preserves submission order. Consecutive sprites are batched only while texture, color, mono mode, filter and blend mode match. There is no texture or Z sort: the last submitted 2D sprite is drawn last, with depth testing/writes disabled.

## Implementation

- Vertices are appended TL, TR, BR, BL. The reusable index buffer is initialized once with the same `0,1,2, 0,2,3` quad pattern as the supplied `glc_quad.cpp`.
- At most 4,096 quads are queued in one batch. Dynamic VB uploads use DISCARD on wrap and NOOVERWRITE for subsequent ranges; the index buffer is managed.
- A one-quad batch uses the previous UP draw path to avoid a VB lock when frequent state changes prevent grouping.
- Changing filter/blend flushes the previous queue. Changing texture/color/mono flushes before the next quad is appended. End flushes the remainder. No groups are merged across intervening draws.
- Deferred textures retain a COM reference until End or replacement. Device loss discards queued work and releases the default-pool VB; reset recreates it.
- Legacy D3DXFont output flushes earlier sprites before drawing. Editing an LcxFont texture flushes previous uses before changing its pixels.
- Raw external D3D rendering or texture writes must call `ILcSpriteX::Flush()` before touching the queued rendering state/resources. Errors from deferred rendering are returned by Flush/End.

## Native regression tests

Run `benchmarks\run-sprite-batch-tests.cmd`. It compiles the current renderer directly and exercises actual D3D9 rendering, not mock draw calls. Every scene is compared pixel-for-pixel with the same scene flushed after each Draw.

Covered cases: consecutive same-state sprites; overlapping A/B/A textures at equal Z; color/filter/blend/mono/texture boundaries; 10,000 sprites across 4,096-quad boundaries; interleaved singleton and VB batches; releasing texture ownership before End; legacy D3DXFont interleaving (including unchanged sprite state after text); editing and releasing A8 font textures mid-frame; and a real device Reset with VB recreation. Batch sizes are also asserted.

## Performance benchmark

Run `benchmarks\run-sprite-benchmark.cmd`. Both scripts currently name this machine's Visual Studio installation path and require the restored Microsoft.DXSDK.D3DX package in `sdk_src/packages`. Output binaries go into the ignored `benchmarks/Release` directory.

Conditions: NVIDIA GeForce RTX 5070 (driver `00200000:00100440`), x64 Release `/O2`, 640 x 480 offscreen target, 8 x 8 RGBA textures, alpha blending, point filtering and screen scale 1. One Begin/End per frame, no sorting. Five warm-up pairs and 21 alternating-order paired samples per condition. The three 1,000-sprite reference images matched D3DXSprite in every RGB channel.

`cpu_median_ms` includes Clear/BeginScene/Begin/Draw/End/EndScene and driver stalls. `completion_median_ms` measures from the same start until a D3D9 event query confirms GPU completion. It is not pure GPU execution time. GPU work is drained and a common baseline state restored before each sample. No Present/vsync is included, so these numbers are rendering cost rather than application FPS.

`sprite-results-immediate.csv` records the earlier renderer before batching. `sprite-results.csv` records the current batched renderer and D3DXSprite. These are separate runs on the same machine; differences include measurement/clock variation.

| 10,000 sprites, completion median | Previous immediate path | Batched path | D3DXSprite in latest run |
| --- | ---: | ---: | ---: |
| Same texture/color | 0.999 ms | 0.383 ms | 1.512 ms |
| Eight alternating textures | 2.513 ms | 2.568 ms | 3.195 ms |
| Changing color every sprite | 1.336 ms | 1.430 ms | 1.506 ms |

Compatible consecutive sprites improve substantially. Texture/color changes on every sprite prevent batching and can incur some queue overhead. Larger images, A8 font performance, transformations and whole-game performance require their own workloads; transformation/A8 correctness is covered by the regression tests.
