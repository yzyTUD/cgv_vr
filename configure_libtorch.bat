@echo off
setlocal EnableDelayedExpansion

echo This script will download and extract the prebuilt binaries from the pytorch website.
echo Download will be approximately 500MB. Approximately 3.5GB of hard drive space is needed after extraction.
:choice
set /P c=Are you sure you want to continue [y/n]?
if /I "%c%" EQU "y" goto :run
if /I "%c%" EQU "n" goto :skip
goto :choice

rem Only execute the script if both the CGV directory and the CGV build directory are specified.

:run
if NOT "%CGV_DIR%" == "" if NOT "%CGV_BUILD%" == "" (
  set script=%CGV_DIR%\3rd\libtorch\get_prebuilt.ps1
  echo Starting script !script! to download libtorch...
  powershell -ExecutionPolicy ByPass -command ". '!script!'
  echo Download and extraction complete.

  set d_src=%CGV_DIR%\3rd\libtorch\dist\libtorch\lib\debug
  set d_dst=%CGV_BUILD%\bin\debug64
  echo Copying required libtorch debug Dlls to !d_dst!...
  robocopy "!d_src!" "!d_dst!" asmjit.dll c10.dll fbgemm.dll libiomp5md.dll torch_cpu.dll /R:3 /W:5 /IS /IT

  set r_src=%CGV_DIR%\3rd\libtorch\dist\libtorch\lib\release
  set r_dst=%CGV_BUILD%\bin\release64
  echo Copying required libtorch release Dlls to !r_dst!...
  robocopy "!r_src!" "!r_dst!" asmjit.dll c10.dll fbgemm.dll libiomp5md.dll torch_cpu.dll /R:3 /W:5 /IS /IT

  echo You can now include "libtorch" in your projects dependencies.
) else (
  echo CGV_DIR and/or CGV_BUILD is not specified. Run define_system_variables first.
)

:skip
pause
