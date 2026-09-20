@echo off
setlocal
cd /d "%~dp0"

set MISSING=0

if not exist "simplewall\simplewall.sln" (
	echo Missing simplewall\simplewall.sln
	set MISSING=1
)
if not exist "routine\src\routine.c" (
	echo Missing routine\src\routine.c
	set MISSING=1
)
if not exist "routine\src\rapp.c" (
	echo Missing routine\src\rapp.c
	set MISSING=1
)
if not exist "builder\build.bat" (
	echo Missing builder\build.bat
	set MISSING=1
)

if "%MISSING%"=="1" (
	echo.
	echo Workspace is not set up. simplewall, routine, and builder must be siblings.
	exit /b 1
)

echo Workspace layout is OK.
echo.
echo   simplewall\  app ^(open simplewall.sln^)
echo   routine\     compiler SDK used via ..\routine
echo   builder\     packaging scripts used via ..\builder
echo.
echo Next:
echo   1. Open simplewall\simplewall.sln in Visual Studio 2022 or 2026
echo   2. Restore NuGet packages
echo   3. Build Release ^| x64
echo.
echo Packaging:  simplewall\build.bat
echo Locales:    simplewall\build_locale.bat
exit /b 0
