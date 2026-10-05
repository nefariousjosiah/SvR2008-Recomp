# SvR 2008 Recompiled

A native PC port of **WWE SmackDown vs. Raw 2008** (Xbox 360), built by statically recompiling the
game's PowerPC code into C++ and compiling it for x86-64. It is not an emulator: the game logic runs
as a native Windows executable. The goal is **native 60 FPS** and modern PC features.

> **Work in progress.** Nothing is released yet. This page tracks progress.

![Title screen running natively on Windows](media/title-screen.png)

*Captured from the recompiled build running natively on Windows (Direct3D 12).*

## How it works

- The game executable (`default.xex`) is translated ahead of time into C++ with the
  [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk) (the successor to XenonRecomp, the tool behind
  Unleashed Recompiled): about 29,000 functions, compiled with Clang.
- The parts of the Xbox 360 outside the game itself (kernel, GPU, XMA audio, file system) are handled by
  ReXGlue's runtime, which builds on the [Xenia](https://github.com/xenia-project/xenia) project's research.
- Because the game code is ordinary C++ at build time, fixes and enhancements (like the frame-rate
  unlock) can be made directly in the code instead of through emulator patches.

## Progress

As of October 5, 2026:

- [x] Recompiles cleanly (0 analysis errors)
- [x] Boots, plays intro movies, reaches the title screen and menus
- [x] Matches playable at original speed on Windows (Direct3D 12)
- [x] Controllers: DualSense, Xbox and other SDL-supported pads, plus keyboard
- [x] Fixed a GPU crash a couple of minutes into matches (still being confirmed in longer sessions)
- [ ] **Native 60 FPS**: unlock the frame limiter and keep game logic at correct speed
- [ ] Full playthrough testing (career, season, create modes)
- [ ] Higher internal resolution, widescreen and ultrawide polish
- [ ] macOS (Apple Silicon): builds and renders, display output still being fixed
- [ ] Loose-file mod support, faster loading, clean removal of defunct Xbox Live features

## FAQ

**Can I download it?** Not yet. When there is a release, it will never include the game's code,
data or executables. You'll need your own copy of the game.

**Is this an emulator?** No. The game's code is translated to C++ before it runs, the same approach as
Unleashed Recompiled. Some Xbox 360 hardware behaviour (GPU, audio) is still reproduced by the runtime.

**Which version?** The Xbox 360 release, "WWE SmackDown vs. Raw 2008" (USA, Europe).

## Credits

- [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk) for the recompiler and runtime
- [Xenia](https://github.com/xenia-project/xenia) for years of Xbox 360 research the runtime builds on
- Yuke's and THQ for the original game

## Legal

This is an unofficial fan project, not affiliated with or endorsed by WWE, THQ, Yuke's, 2K or Microsoft.
All trademarks belong to their owners. This repository contains no game code, assets or executables.
