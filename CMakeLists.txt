cmake_minimum_required(VERSION 3.25)

# vcpkg installe les dépendances listées dans vcpkg.json.
# VCPKG_ROOT est défini par les invites de commande développeur de Visual Studio.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

project(Passages LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(TIFF REQUIRED)
find_package(JPEG REQUIRED)

# BASS (un4seen.com) n'est pas dans vcpkg : le SDK officiel est téléchargé à la configuration.
include(FetchContent)
FetchContent_Declare(bass
    URL https://www.un4seen.com/files/bass24.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(bass)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BASS_ARCH "x64/")
endif()
add_library(bass SHARED IMPORTED)
set_target_properties(bass PROPERTIES
    IMPORTED_LOCATION "${bass_SOURCE_DIR}/${BASS_ARCH}bass.dll"
    IMPORTED_IMPLIB "${bass_SOURCE_DIR}/c/${BASS_ARCH}bass.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${bass_SOURCE_DIR}/c")

add_executable(Passages main.cpp Passages.manifest)

target_link_libraries(Passages PRIVATE
    OpenGL::GL OpenGL::GLU GLEW::GLEW glfw imgui::imgui TIFF::TIFF JPEG::JPEG bass)

target_compile_definitions(Passages PRIVATE
    _CRT_SECURE_NO_WARNINGS
    _USE_MATH_DEFINES
    PASSAGES_DONNEES="${CMAKE_SOURCE_DIR}")

target_compile_options(Passages PRIVATE /utf-8)

# La démo charge ses données (textures, modèles, shaders...) depuis la racine du projet.
set_target_properties(Passages PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Copie les DLL à côté de l'exécutable.
add_custom_command(TARGET Passages POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:Passages> $<TARGET_FILE_DIR:Passages>
    COMMAND_EXPAND_LISTS)
