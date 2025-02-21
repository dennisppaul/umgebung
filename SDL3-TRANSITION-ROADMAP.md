# SDL3 Transition Roadmap

1. clean up core Umgebung and merge with PApplet
2. implement flexible renderer selection. future renders may include:
    1. OpenGL 2.0 ( fixed function pipeline )
    2. OpenGL 3.3
    3. OpenGL ES 3.0
    4. SDL_gpu ( Vulkan, Metal + maybe Direct3D )
    5. SDL_render ( 2D only )
3. port current *OpenGL 2.0* to *OpenGL 3.3*
4. transition to SDL3

## References

- consult [SDL3 Wiki](https://wiki.libsdl.org/SDL3/FrontPage)
- consult [Migrating to SDL 3.0](https://github.com/libsdl-org/SDL/blob/main/docs/README-migration.md).
- apply *core changes* ( see below )

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

## research SDL3 features

see [New Features in SDL3](https://wiki.libsdl.org/SDL3/NewFeatures)

- [video capture / camera](https://wiki.libsdl.org/SDL3/CategoryCamera) @question(is it based on another camera/video library liek ffmpeg?)
- implement `Camera` from this example ( with audio ) https://github.com/libsdl-org/SDL/blob/17549435960cf114a5118f837e0a71d6e7e0d4c9/test/testffmpeg.c
- GPU API
- Enhanced HiDPI Support
- consider switching to [main callbacks](https://wiki.libsdl.org/SDL3/README/main-functions)
- [Web Browser Examples](https://github.com/libsdl-org/SDL/tree/main/examples) and [SDL_main.h](https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_main.h)

## additional changes

- clean up `CMakeLists.txt`
- update installation files + documentation to SDL3
    - `Brewfile`
    - `install-RPI.sh`
    - `DOCUMENTATION.md`
    - `README.md`

## improvements or non-SDL3 related changes

### en-/disabling features

currently it is possible to enable or disable certain features like graphics, sound or movies at *compile time* via CMake options, while also providing a *stub** implementation which allows copmilation ( and ideally exection ) even when the feature is disabled ( e.g to quickly be able to make an application *headless* ). check if this strategies introduces too much overhead and if there is a less cleaner way ( e.g escaping `#ifdef` hell ) to implement this.
