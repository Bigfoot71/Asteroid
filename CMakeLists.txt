cmake_minimum_required(VERSION 3.0)
project(Asteroid)

set(CMAKE_CXX_STANDARD 17)

# Also include 'raylib/cmake/LibraryConfigurations' here
# to obtain GRAPHICS and PLATFORM_CPP definitions
include(external/rayFlex/external/raylib/cmake/LibraryConfigurations.cmake)
if (NOT PLATFORM_CPP)
    set(PLATFORM_CPP "PLATFORM_DESKTOP")
endif()

# Configuration for compiling with emscripten (wasm)
if("${PLATFORM_CPP}" STREQUAL "PLATFORM_WEB")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    configure_file(${CMAKE_SOURCE_DIR}/html/shell.php ${CMAKE_BINARY_DIR}/shell.php COPYONLY)       # Version allowing SharedArrayBuffer
    configure_file(${CMAKE_SOURCE_DIR}/html/shell.html ${CMAKE_BINARY_DIR}/shell.html COPYONLY)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --shell-file ${CMAKE_BINARY_DIR}/shell.html --preload-file assets.rres -sINITIAL_MEMORY=131072000 -sWASM=1 -sASYNCIFY=1 -sUSE_GLFW=3")
endif()

# Configure the 'rayflex' library
add_subdirectory(external/rayFlex)

# Include 'Asteroid' sources
include(source/CMakeLists.txt)

# Pre-configure to compile for Windows
if(WIN32)
    configure_file(version.rc.in ${CMAKE_BINARY_DIR}/version.rc @ONLY)
endif()

# Confiugre 'Asteroid' executable
add_executable(${PROJECT_NAME} ${ASTEROID_SOURCE})

# Specify include paths for 'Asteroid'
target_include_directories(${PROJECT_NAME} PUBLIC
    external/rres/src include
)

# Specify libraries to link with 'Asteroid'
target_link_libraries(${PROJECT_NAME} PUBLIC rayflex)

# Configuration to compile for Windows
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC winmm)                     # build issue with external (?)
    target_link_options(${PROJECT_NAME} PRIVATE "-mwindows")                # to hide Windows console (cmd)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE true)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.ico ${CMAKE_BINARY_DIR}/icon.ico  COPYONLY)
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/version.rc)
endif()

# Definitions of macros for 'Asteroid'
target_compile_definitions(${PROJECT_NAME} PUBLIC
    "${PLATFORM_CPP}" "${GRAPHICS}" SUPPORT_GFX_2D
)

# Copy assets to the build directory
configure_file(
    ${CMAKE_SOURCE_DIR}/assets/assets.rres
    ${CMAKE_BINARY_DIR}/assets.rres
    COPYONLY
)
