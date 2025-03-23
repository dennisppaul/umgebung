# SDL3 Transition Roadmap

1. [x] clean up core Umgebung and merge with PApplet
2. [x] implement flexible renderer selection. future renders may include:
    1. [x] OpenGL 2.0 ( fixed function pipeline )
    2. [x] OpenGL 3.3
    3. [ ] OpenGL ES 3.0
    4. [ ] SDL_gpu ( Vulkan, Metal + maybe Direct3D )
    5. [x] SDL_render ( 2D only )
3. [x] port current *OpenGL 2.0* to *OpenGL 3.3*
4. [x] transition to SDL3
5. [ ] add imgui
6. [ ] add more SDL functions
7. [ ] test platforms ( e.g RPI + iOS + WASM )

## Renderer Status and TODOs

### OpenGL 3.3

- [x] implement atlas-based font rendering ( with FreeType )
- [x] add *immediate* mode
- [ ] add dedicated bins for transparent and non-transparent primitives in *retained* mode
- [x] MSAA FBOs

### SDL_Renderer

currently not more than a *proof of concept*. need to add texture ( and font ) rendering.

### SDL_gpu

- https://github.com/TheSpydog/SDL_gpu_examples
- [SDL 3 GPU tutorial with C++23 and Modern CMake (Part 1)](https://www.youtube.com/watch?v=UFuWGECc8w0)
- [SDL 3 GPU tutorial with C++23 and Modern CMake (Part 2)](https://www.youtube.com/watch?v=HYb753diRYA)
- [SDL 3 GPU tutorial with C++23 and Modern CMake (Part 3)](https://www.youtube.com/watch?v=ddi7V0CDkLQ)

## core changes

graphics, audio and maybe camera should or could be updated to SDL3. discuss, conceptualize, and merge/resolve:

- `Umgebung.h`
- `Umgebung.cpp`
- `UmgebungConstants.h`
- `UmgebungConstants.cpp`
- `UmgebungFunctions.h`
- `UmgebungFunctions.cpp`
- `UmgebungGraphics*`

with 

- `PApplet.h`
- `PGraphics.h`

there is an inconsistency with the way functions and graphics ( contexts ) are handled in the *original* processing. BTW to a lesser degree the same is true for the audio context. there it would also be nice to more or less seamless switch between drivers ( e.g SDL or portaudio ).

### main function

see [Where an SDL program starts running.](https://github.com/libsdl-org/SDL/blob/main/docs/README-main-functions.md)

## Transition to OpenGL 3.3 and/or SDL_gpu

consider transitioning to OpenGL 3.3 ( including OpenGL ES for mobile platforms ) but also consider the option to completely stay away from OpenGL and revert to SDL3_gpu in order to make the render pipeline more futureproof ( apple might abandon OpenGL some time soon ) as well as potentially more platform independent.

## En-/Disabling features

currently it is possible to enable or disable certain features like graphics, sound or movies at *compile time* via CMake options, while also providing a *stub** implementation which allows copmilation ( and ideally exection ) even when the feature is disabled ( e.g to quickly be able to make an application *headless* ). check if this strategies introduces too much overhead and if there is a less cleaner way ( e.g escaping `#ifdef` hell ) to implement this.

## Additional Changes

- clean up `CMakeLists.txt`
- update installation files + documentation to SDL3
    - `Brewfile`
    - `install-RPI.sh`
    - `DOCUMENTATION.md`
    - `README.md`