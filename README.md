# Passages_Unfinished

Unfinished demo project "PASSAGES" - Genesis Project 2012  
C++ / OpenGL  
Code / Design / Modeling : Erk  
GFX : Razorback  
Musics : HJE - Sonic Wandered  
[![](work/Passages.png)](https://youtu.be/IB4lxcsACjc)

## Building

Requires Windows and Visual Studio 2026 with the *Desktop development with C++* workload (it includes CMake and vcpkg).

From a *Developer PowerShell for VS 2026*, at the root of the repository:

    cmake --preset vs2026
    cmake --build --preset release

The first configuration takes a few minutes: vcpkg builds freeglut, GLEW, libtiff and libjpeg-turbo, and the BASS audio library is downloaded from [un4seen.com](https://www.un4seen.com).

Run `build\Release\Passages.exe`. The demo loads its textures, models and shaders from the repository root, wherever the executable is started from.
