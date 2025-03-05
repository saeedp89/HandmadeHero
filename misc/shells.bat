@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Auxiliary\Build\vcvars64.bat"
call "C:\Program Files\Microsoft Visual Studio\2022\Preview\Common7\Tools\VsDevCmd.bat"
SUBST W: C:\Users\saeed\Projects\game_dev
set path=W:\misc;%path%
PROMPT $C$T$F$S$P$_$B$G$S
