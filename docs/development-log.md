# WWE SmackDown vs. Raw 2008 — Xbox 360 static recompilation

Native PC/Mac port of the **Xbox 360** version of SvR 2008, made by statically recompiling
`default.xex` (PowerPC/Xenon) into C++ with [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk).
The goal is **native 60 fps**.

ReXGlue is the successor to XenonRecomp/XenosRecomp (the tools behind Unleashed Recompiled).
Its runtime comes from Xenia: kernel (xboxkrnl/xam), Xenos GPU (D3D12 on Windows, Vulkan →
MoltenVK on macOS), XMA audio, VFS and input. We write only the game-specific glue:
function boundaries, hooks, and the frame-rate work. The PS2 pipeline from `WWE2006`
(PS2Recomp) does not apply here.

## Layout

| Path | What it is |
| --- | --- |
| `iso/` | The extracted `.iso` (from the Redump `(USA, Europe)` 7z) |
| `assets/` | Disc contents (`default.xex` + data); the runtime's `game:\` / `d:\` root |
| `third_party/rexglue-sdk` | ReXGlue SDK v0.10.0 (`c94f5eb`) with all submodules |
| `third_party/rexglue-sdk.wiki` | SDK docs (partly behind v0.10's CLI; check `rexglue <cmd> --help`) |
| `third_party/XEXLoaderWV` | Ghidra XEX loader (SaveEditors fork `ab36b29`), patched for Ghidra 12.1 |
| `sdk/` | Installed SDK build (`sdk/bin/rexglue`, CMake package, registered in `~/.cmake/packages`) |
| `tools/` | Pipeline scripts (below) |
| `ghidra/`, `ghidra_scripts/` | Ghidra project for finding the frame limiter etc. |
| `logs/` | SDK build logs, codegen and run logs |

Created by `tools/setup_project.sh` (`rexglue init`): `CMakeLists.txt`, `CMakePresets.json`,
`svr2008_manifest.toml`, `src/main.cpp`, `src/svr2008_app.h`, `generated/`.

## Toolchain (already installed)

- Homebrew: `cmake`, `ninja`, `vulkan-loader`, `molten-vk`, `sevenzip` (`7zz`), `gradle`
- Apple Clang 17 (ReXGlue requires Clang; on Windows, VS 2022 + clang-cl 20+)
- Ghidra 12.1.4 + `XEXLoaderWV` in `~/Library/ghidra/ghidra_12.1.4_PUBLIC/Extensions/`

## Pipeline

```bash
tools/import_disc.sh        # 1. 7z -> iso/ -> assets/ (XDVDFS extract), records default.xex SHA-1
tools/setup_project.sh      # 2. rexglue init (one-time)
tools/regen.sh              # 3. codegen default.xex -> generated/  (FORCE=1 to emit despite unresolved calls)
tools/build.sh              # 4. cmake preset mac-arm64-release (PRESET=win-amd64-release on Windows)
./run.sh                    # 5. run; F3 = FPS/debug overlay, ` = console, F4 = CVar settings
tools/ghidra_import.sh      # (one-time) Ghidra project for reverse engineering
```

`tools/build_sdk.sh` rebuilds `sdk/` after changes to `third_party/rexglue-sdk`.

### Windows (D3D12)

`tools/package_windows.sh` zips the project (no builds/SDK/game data) to `~/Desktop/WWE2008-windows.zip`.
On the PC, unzip it, put the game `.7z` in the project folder (or pass `-Archive`), then:

```powershell
powershell -ExecutionPolicy Bypass -File tools\windows\setup.ps1
powershell -ExecutionPolicy Bypass -File tools\windows\run.ps1
```

`setup.ps1` installs Git/CMake/Ninja/LLVM/Python/7-Zip/VS 2022 Build Tools with winget, clones the SDK
at the same commit, builds it into `sdk\`, unpacks the disc, runs codegen and builds `svr2008.exe`.
Each step is skipped when already done, so it can be re-run after a failure.

Day to day (Windows equivalents of `regen.sh` / `build.sh`; both set up VS 2022 x64 + LLVM clang via `env.ps1`):

```powershell
powershell -ExecutionPolicy Bypass -File tools\windows\regen.ps1   # codegen only, log in logs\codegen.log
powershell -ExecutionPolicy Bypass -File tools\windows\build.ps1   # configure + build (re-runs codegen if inputs changed)
powershell -ExecutionPolicy Bypass -File tools\windows\run.ps1     # play; log in logs\game.log
```

Toolchain on the PC: LLVM clang 23 (`C:\Program Files\LLVM`, GNU-style `clang++`, not clang-cl), VS 2022's
Windows SDK/linker/Ninja, CMake 3.25+. No Vulkan SDK needed; Windows renders through D3D12.

Codegen is not one-click. Expect to iterate on `svr2008_manifest.toml`:
- `[functions]`: boundaries for anything analysis gets wrong
- `[[switch_tables]]`: jump tables auto-detection misses
- `[[midasm_hook]]` / `REX_HOOK`: patches and fixes (see the wiki's *Function-Overrides* and *Mid-ASM-Hooks*)
- missing kernel imports implemented in `src/`

## Plan to 60 fps

1. **Boot at stock speed first.** Get codegen clean, then get past the kernel/XAM, GPU and
   audio gaps until menus and a match run. Use the F3 overlay for guest vs. host FPS.
2. **Find the frame cap.** 360 games limit FPS through the presentation interval: D3D's
   `PresentInterval` (2 = 30 fps), which the game drives via `VdSwap` and waits on the vblank
   interrupt / `VdGetCurrentDisplayInformation`. In Ghidra, trace xrefs to the `VdSwap`,
   `VdSetDisplayMode` and `KeWaitForSingleObject` imports up to the game's present/flip
   function and its vblank-wait loop. The Xenia patch DB has no SvR 2008 entry (only
   *WWE All Stars*, `54510866`), so this is new work.
3. **Check whether game logic is tied to frames.** Yuke's games usually step the simulation
   once per frame. If so, removing the cap makes the game run at double speed. Options, from
   least to most work:
   - if the game already uses a delta/timestep variable, set it to 1/60;
   - run the simulation at 30 Hz and render at 60, interpolating camera and bone matrices
     between ticks (hooks on the sim-tick and render-submit functions);
   - per-system fixes (animation playback rate, physics, timers, audio sync, FMVs).
4. **Wire it up as hooks** in `src/` and the manifest (`[[midasm_hook]]` on the vblank wait,
   `REX_HOOK` on the tick function), behind a CVar so 30/60 can be toggled.
5. Host side: `--vsync`, `vulkan_allow_present_mode_mailbox`, and
   `d3d12_allow_variable_refresh_rate_and_tearing` already exist as runtime CVars.

## Local changes to third-party code

- `rexglue-sdk`: kept in `patches/rexglue-sdk.patch` (applied by `tools\windows\setup.ps1`; regenerate with
  `git -C third_party/rexglue-sdk diff --output=patches/rexglue-sdk.patch`). From the first Windows
  playtest, where matches died ~2 min in with a D3D12 `DEVICE_HUNG` + GPU page fault:
  - DRED breadcrumbs/page-fault tracking always on (was debug-layer only);
  - a device loss first seen by the presenter now logs the removal reason, DRED data and the
    live/recently-freed allocations at the faulting VA, and flushes the log before aborting;
  - texture cache evictions are logged;
  - the shared memory buffer's GPU VA range is logged;
  - with `--d3d12_debug=true`, D3D12 debug layer messages are forwarded to the log;
  - `--d3d12_gpu_validation` cvar (GPU-based validation) and shader-visible descriptor heap ranges logged;
  - async shader compilation race fix (see the open issue below).
  - Performance (see "Heavy scenes" below): XMP polling throttle only for threads hammering it;
    Vulkan reuses a stage's texture descriptor set when a draw binds the same views and samplers;
    `SharedMemory::RequestRanges` reuses its scratch vector; the GPU command thread runs above normal
    priority (`--gpu_thread_above_normal_priority`, default on).
- `XEXLoaderWV`: replaced `org.python.jline.internal.Log` (no longer shipped with Ghidra 12.1)
  with a local `xexloaderwv/Log.java` shim backed by `ghidra.util.Msg`.

### Open issue: GPU page fault ~2 min into matches (Windows, RX 6650 XT)

`DEVICE_HUNG` with a DRED page fault. Findings so far (2026-10-05):
- Third crash: the faulting op was a `DrawIndexedInstanced` in flight (DRED op type 4), VA `0x348140000`;
  the earlier one was `0x348BC0000`. DRED matched no live or recently freed allocation at either VA.
- Not texture eviction: the third crash had texture cache limits raised and zero evictions logged.
  (`run.ps1` still raises the limits; harmless.)
- Not tiled shared memory: the 512 MB shared memory buffer sat at `0x301180000-0x321180000`,
  well below the fault addresses.
- Render target path on AMD is host RTVs (Xenia's default for AMD).
- Fourth crash (debug layer on): page fault at `0x347200000`; the debug layer reported nothing before
  the removal, so the D3D12 API calls themselves are valid; the bad address comes from what shaders read.
- Fixed in passing: with async shader compilation, a draw could bind the placeholder (VS-only) root
  signature with the final pipeline if compilation finished between `ConfigurePipeline` and the
  readiness check (`command_processor.cpp`); the root signature is now re-read once the state exists.
  The debug layer's root-signature-mismatch check stayed silent, so this probably wasn't the crash.
- GPU-based validation (`--d3d12_gpu_validation=true`, new cvar; `Diagnose crash (slow).bat`) made the
  game too slow to reach a match, so it has not produced a result yet.
- Fifth/sixth crashes: a non-indexed `DrawInstanced` faulted too (so not the index buffer), at
  `0x341AC0000` and `0x3470C0000`, the latter with `--async_shader_compilation=false
  --gpu_3d_to_2d_texture=false --d3d12_bindless=false` (so none of those three).
- Not a descriptor heap overrun: shader-visible heaps sit at `0x100xxxxxx` on this AMD driver
  (logged at creation now).
- All fault VAs are 64 KB-aligned and in the region where resources are allocated during play.
- **Workaround: the Vulkan backend.** The SDK is now built with `-DREXGLUE_USE_VULKAN=ON` and our app
  takes `--gpu_backend=vulkan|d3d12` (`src/svr2008_app.h`); `run.ps1` defaults to Vulkan. First
  Vulkan session: 5.5 minutes, no GPU errors, closed normally.

## 60 fps work

**Root cause found and fixed (2026-10-05):** sampling the frame thread's call stack with lldb showed
it sleeping inside `XmpApp::DispatchMessageSync` 28 of 30 times. The runtime (from Xenia) sleeps 1 ms
in `XMPGetStatus` and 10 ms in `XMPGetPlaybackController` on any non-main thread, for games that poll
them in a tight loop; SvR 2008 polls a few times per frame from its frame thread, losing up to 10+ ms
of every 16.7 ms frame. The patch now only throttles threads calling more than ~1000 times per second.
Title screen/menus went from ~46 fps to a locked 60.0. Earlier notes below.

**Correct speed at 60 (2026-10-05):** overriding only the D3D present interval made entrances run
at double speed: the game has a frame-rate *mode* (global struct `0x82E6E458`: +8 target fps
60/50/30/25, +28/+32/+56/+60 time-step floats, +36 ms per frame) read by ~140 functions and set by
SetFrameRate `sub_824707A8`, "drop to 30" `sub_82470A18` (2 call sites) and "back to 60"
`sub_82470AC0` (4 call sites); `sub_82471190` maps 30/25 to present interval 2. With
`--svr_60fps=true`, `src/fps_hooks.cpp` skips the drop to 30 and turns SetFrameRate(30/25) into
60/50, so drawing and game timing stay in step; the present-interval hook now only logs.

- **Cap found:** the game's statically linked D3D Swap (`sub_8223A4B8`) reads the present interval
  (device+13572) at `0x8223A56C`; the flip scheduler (`sub_8223A320`) schedules each flip at
  "last flip + interval" vblanks (vblank handler `sub_8223A220`, counter device+16524). Menus ask
  for 1 (60 fps); matches ask for 2 (30 fps). `src/fps_hooks.cpp` (`[[midasm_hook]]` in
  `config/default.toml`) logs it and, with `--svr_60fps=true`, turns 2 into 1.
- **Result:** matches went from 30.0 to a steady ~40 fps of new frames at **normal game speed**
  (logic looks time-based). Menus run ~46 fps even though they ask for 60.
- **Bottleneck is waiting, not work:** the GPU thread spends ~0.25 ms/frame translating commands and
  ~10 ms in `WAIT_REG_MEM` on a CPU-written fence (`mem 0x14AF5006`); the present thread (calls
  `VdSwap`) waits on one event per frame; the frame-producing thread (waits on `0xF8000070` from
  `0x82446920`) uses ~1% CPU and is in a kernel wait only ~13% of the time, so it is blocked
  host-side ~85% of the time (suspect: a runtime lock / critical section, not covered by the probe).
- The profiling used for this (guest FPS, draws per swap, per-wait stats, kernel wait probe) has been
  removed from the runtime patch again.

## Heavy scenes (crowd shots), 2026-10-05

- Entrance camera shots of the crowd issue up to ~5,250 draws per frame (each fan is drawn
  separately). The GPU command thread translates every draw on one thread at ~2.2-2.4 us each on a
  Ryzen 7 5800X3D, so those frames sit close to the 16.7 ms budget at 1440p.
- Frame drops below 60 in those shots were seen only at 1440p *while recording with OBS using x264*
  (software encoding on the CPU, plus Display Capture and Game Capture both active). With another
  recorder they went away. Recording advice: use a hardware encoder (AMD/NVIDIA/Intel), Game Capture only.
- The game's own D3D (`sub_822386B8`, BlockOnFence, polling via `sub_82239E68` with `db16cyc` only)
  busy-waits for the GPU fence once per frame, so the main game thread looks ~90% busy while it is
  mostly waiting on the GPU command thread. Possible later improvement: a midasm hook there that
  pauses/yields, to free that core on CPUs with fewer cores and on laptops.
- Kept optimizations: texture descriptor set reuse (bindings ~0.54 -> ~0.51 us/draw), no per-call
  allocation in `RequestRanges`, GPU thread above normal priority. Portable: no CPU-specific builds
  (SDK stays `-march=x86-64-v2`, game `-msse4.1`), so it runs on the same range of PCs as before.

## License

The original code of this project is licensed under the [MIT License](LICENSE), copyright (c) 2026
nefariousjosiah. That covers:

- `tools/` (pipeline, build, launch and diagnostic scripts) and the `.bat` launchers
- `src/` (the app: window title, GPU backend selection, frame capture)
- `config/` (codegen function lists and exclusions) and `svr2008_manifest.toml`
- `ghidra_scripts/`
- this README and other documentation

It does **not** cover:

- **The game.** `default.xex`, the disc contents (`assets/`, `iso/`), the recompiled C++ in
  `generated/default/` and any built executable are derived from WWE SmackDown vs. Raw 2008, which belongs
  to its copyright holders (THQ, Yuke's, WWE). None of it may be distributed.
- **ReXGlue SDK** (`third_party/rexglue-sdk`, `sdk/`): BSD-3-Clause, by Tom Clay and the Xenia authors.
  `patches/rexglue-sdk.patch` modifies that code and stays under BSD-3-Clause with their notices.
- **Build boilerplate generated by `rexglue init`** (`generated/rexglue.cmake` and the original
  `CMakeLists.txt` / `CMakePresets.json` skeletons) comes from ReXGlue's templates (BSD-3-Clause).
- **XEXLoaderWV** and other third-party tools keep their own licenses.
- **Press Start 2P** (`res/fonts/`, the FPS counter font) is under the SIL Open Font License 1.1
  (`res/fonts/OFL.txt`).

Playing requires your own legally obtained copy of the game. This project does not condone piracy.
