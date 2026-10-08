@echo off
rem Native renderer development: play normally (current renderer) while the game records data for
rem the native renderer - every shader it creates (logs\shaders) and which D3D functions it calls
rem (the [census] lines in logs\game.log). Uses this folder's own copy of the save.
set "SVR_DUMP_SHADERS=%~dp0logs\shaders"
start "WWE SVR 2008 (native dev) - log" powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\windows\run.ps1" --fullscreen=true --resolution_scale=2 %*
