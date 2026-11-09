@echo off
chcp 65001 >nul
rem ==========================================================================
rem   Compile Passages en Debug, puis lance la démo.
rem   Les paramètres sont transmis à la démo, par exemple :
rem       compile_debug.bat --scene tunnel
rem   Nécessite Visual Studio 2026 avec le développement Desktop en C++.
rem ==========================================================================
setlocal
pushd "%~dp0"

rem --- Visual Studio 2026 (CMake et vcpkg sont fournis avec) :
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" goto pas_de_visual_studio
set "VS="
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -version 18.0 -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS=%%i"
if not defined VS goto pas_de_visual_studio
set "CMAKE=%VS%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if not defined VCPKG_ROOT set "VCPKG_ROOT=%VS%\VC\vcpkg"

rem --- Configuration, la première fois (vcpkg compile alors les bibliothèques : quelques minutes) :
if not exist "build\CMakeCache.txt" (
    echo Configuration du projet...
    "%CMAKE%" --preset vs2026 || goto erreur_compilation
)

rem --- Compilation :
echo Compilation en Debug...
"%CMAKE%" --build --preset debug -- -v:minimal -nologo || goto erreur_compilation

rem --- Lancement (la console garde les messages de la démo) :
echo Lancement de la démo...
"build\Debug\Passages.exe" %*
set "CODE=%errorlevel%"
if not "%CODE%"=="0" (
    echo.
    echo La démo s'est terminée avec le code %CODE%.
    pause
)
popd
exit /b %CODE%

:pas_de_visual_studio
echo Visual Studio 2026 avec le développement Desktop en C++ est introuvable.
goto fin_erreur

:erreur_compilation
echo.
echo La compilation a échoué : voir les messages ci-dessus.

:fin_erreur
popd
pause
exit /b 1
