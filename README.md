# Asteroid
A small and cool project created for the raylib Slo-Jam 2023-2024 game jam!
You can find its Itch.io page [here](https://bigfoot71.itch.io/asteroid)!

## Building
You can build this project for Windows, Linux, and Web using CMake.

1. Clone the repository with all its submodules, including [rayFlex](https://github.com/Bigfoot71/rayFlex) and [rres](https://github.com/raysan5/rres) _(rayFlex contains raylib)_:
```console
git clone --recursive https://github.com/Bigfoot71/Asteroid.git
```

2. Enter the cloned directory and create a build directory, then enter it:
```console
cd Asteroid && mkdir build && cd build
```

### Build for Desktop

To build for desktop, configure the CMake project as follows:
```console
cmake .. -DSUPPORT_GFX_2D=ON
```

### Build for Web

To build for the web, you will need Emscripten and specify the path to the CMake toolchain module provided with Emscripten:
```console
cmake .. -DSUPPORT_GFX_2D=ON -DPLATFORM=Web -DCMAKE_TOOLCHAIN_FILE=/path/to/your/emscripten/cmake/Modules/Platform/Emscripten.cmake
```
