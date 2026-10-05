# SvR 2008 Recompiled

A native PC port of **WWE SmackDown vs. Raw 2008** (Xbox 360), built by statically recompiling the
game's PowerPC code into C++ and compiling it for x86-64. It is not an emulator: the game logic runs
as a native Windows executable. The goal is **native 60 FPS** and modern PC features.

> **Work in progress.** Nothing is released yet. This page tracks progress.

> [!IMPORTANT]
> **You must own the game to play this.** This project will never include or distribute the game's
> code, data or executables. To use it, you'll need your own legally purchased Xbox 360 copy of
> WWE SmackDown vs. Raw 2008, dumped from your own disc. **This project does not condone piracy.**
> Requests for game files, ISOs or download links will be closed without reply.

![Title screen running natively on Windows](media/title-screen.png)

*Captured from the recompiled build running natively on Windows (Direct3D 12).*

## How it works

- The game executable (`default.xex`) is translated ahead of time into C++ with the
  [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk) (the successor to XenonRecomp, the tool behind
  Unleashed Recompiled): about 29,000 functions, compiled with Clang.
- The parts of the Xbox 360 outside the game itself (kernel, GPU, XMA audio, file system) are handled by
  ReXGlue's runtime, which builds on the [Xenia](https://github.com/xenia-project/xenia) project's research.
  On Windows, the game's graphics are rendered through Vulkan (Direct3D 12 is also built in).
- Because the game code is ordinary C++ at build time, fixes and enhancements (like the frame-rate
  unlock) can be made directly in the code instead of through emulator patches.

## Progress

As of October 5, 2026:

- [x] Recompiles cleanly (0 analysis errors)
- [x] Boots, plays intro movies, reaches the title screen and menus
- [x] Matches playable at original speed on Windows, with no crashes, using the Vulkan renderer
- [x] Controllers: DualSense, Xbox and other SDL-supported pads, plus keyboard
- [x] Worked around a GPU crash 1–2 minutes into matches by switching from Direct3D 12 to Vulkan
      (the Direct3D 12 bug itself is still being investigated)
- [x] Windowed mode, plus a launcher with a live log window for crash reports
- [ ] **Native 60 FPS**: unlock the frame limiter and keep game logic at correct speed
- [ ] Full playthrough testing (career, season, create modes)
- [ ] Higher internal resolution, widescreen and ultrawide polish
- [ ] macOS (Apple Silicon): builds and renders, display output still being fixed
- [ ] Loose-file mod support, faster loading, clean removal of defunct Xbox Live features

## FAQ

**Can I download it?** Not yet. When there is a release, it will never include the game's code,
data or executables. You'll need your own legally purchased copy of the game, dumped from your own
disc.

**Can you share the game files or an ISO?** No. This project does not condone piracy and will never
host, share or link to game files. Issues or messages asking for them will be closed.

**Is this an emulator?** No. The game's code is translated to C++ before it runs, the same approach as
Unleashed Recompiled. Some Xbox 360 hardware behaviour (GPU, audio) is still reproduced by the runtime.

**Which version?** The Xbox 360 release, "WWE SmackDown vs. Raw 2008" (USA, Europe).

## Credits

- [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk) for the recompiler and runtime
- [Xenia](https://github.com/xenia-project/xenia) for years of Xbox 360 research the runtime builds on
- Yuke's and THQ for the original game

## Legal

This is an unofficial fan project, not affiliated with or endorsed by WWE, THQ, Yuke's, 2K or Microsoft.
All trademarks belong to their owners. This repository contains no game code, assets or executables,
and it never will. Playing requires a legitimately owned copy of the original game. Piracy is not
supported or condoned.
