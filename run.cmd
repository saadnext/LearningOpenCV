@echo off
setlocal EnableExtensions

rem --------------------------------------------------------------------------
rem  Launch the ASCII art generator with an image path.
rem
rem  Usage:
rem    run.cmd <image>
rem    run.cmd --help
rem
rem  Works from any directory and from Explorer: drop an image file onto this
rem  script and its path is passed as the first argument.
rem
rem  If no executable has been built yet, this script builds Release x64 once
rem  with MSBuild, then runs it. Arguments and the exit code are forwarded to
rem  the program unchanged (0 = ok, 1 = image could not be loaded,
rem  2 = invalid command line).
rem --------------------------------------------------------------------------

set "ROOT=%~dp0"

call :find_exe
if not defined EXE call :build
if not defined EXE exit /b 1

rem Let the program find the OpenCV runtime DLL if it was dropped here.
if exist "%ROOT%Dependencies\OpenCV\bin" set "PATH=%ROOT%Dependencies\OpenCV\bin;%PATH%"

"%EXE%" %*
set "RC=%ERRORLEVEL%"
if "%RC%"=="-1073741515" goto :missing_dll
exit /b %RC%


:find_exe
rem Prefer Release over Debug, and a solution-level build over a project-level
rem one (Visual Studio uses a different output folder for each).
set "EXE="
for %%C in (Release Debug) do (
    if not defined EXE if exist "%ROOT%x64\%%C\LearningOpenCV.exe" set "EXE=%ROOT%x64\%%C\LearningOpenCV.exe"
    if not defined EXE if exist "%ROOT%LearningOpenCV\x64\%%C\LearningOpenCV.exe" set "EXE=%ROOT%LearningOpenCV\x64\%%C\LearningOpenCV.exe"
)
exit /b 0


:build
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" goto :no_msbuild

set "MSBUILD="
for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do set "MSBUILD=%%I"
if not defined MSBUILD goto :no_msbuild

echo Building Release x64 (first run only)...
"%MSBUILD%" "%ROOT%LearningOpenCV.sln" /nologo /m /v:minimal /p:Configuration=Release /p:Platform=x64 > "%TEMP%\LearningOpenCV-build.log" 2>&1
if errorlevel 1 goto :build_failed

call :find_exe
if not defined EXE goto :build_failed
exit /b 0

:build_failed
echo.
echo Build failed. Full output is in "%TEMP%\LearningOpenCV-build.log":
echo.
type "%TEMP%\LearningOpenCV-build.log"
exit /b 1

:no_msbuild
echo Could not find MSBuild. Open LearningOpenCV.sln in Visual Studio and build
echo Release x64, or run:
echo   msbuild LearningOpenCV.sln /p:Configuration=Release /p:Platform=x64
exit /b 1

:missing_dll
echo.
echo The program could not start: the OpenCV runtime DLL was not found.
echo Copy "opencv_world4120.dll" (Release) or "opencv_world4120d.dll" (Debug)
echo either next to the executable or into:
echo   %ROOT%Dependencies\OpenCV\bin
exit /b 1
