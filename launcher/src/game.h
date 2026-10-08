// The game this launcher starts. The only file that differs between the games' launchers.
#pragma once

#include <cstdint>

namespace launcher::game {

inline constexpr const char *kId = "svr2008";  // svr2008.exe, svr2008.toml, svr2008-game.txt
inline constexpr const char *kTitle = "WWE SmackDown vs. Raw 2008";
inline constexpr const char *kYear = "2008";
// The recompiled program only runs this exact disc release (default.xex execution info).
inline constexpr uint32_t kTitleId = 0x5451080B;
inline constexpr uint32_t kMediaId = 0x5E35F037;
inline constexpr const char *kRelease = "USA / Europe";
inline constexpr const char *kDiscSize = "7.8 GB";
// Card colours: gradient and accent (buttons, glow).
inline constexpr uint8_t kArtA[3] = {24, 66, 170};
inline constexpr uint8_t kArtB[3] = {160, 26, 36};
inline constexpr uint8_t kAccent[3] = {64, 120, 255};
inline constexpr uint8_t kAccentText[3] = {255, 255, 255};  // text on accent buttons

}  // namespace launcher::game
