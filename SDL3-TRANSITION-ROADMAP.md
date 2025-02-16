# SDL3 Transition Roadmap

- read the [SDL3 Wiki](https://wiki.libsdl.org/SDL3/FrontPage)
- create simple examples for graphics (OpenGL + SDL drawing lib), audio, camera etcetera
- apply *core changes* ( see below )

consult [Migrating to SDL 3.0](https://github.com/libsdl-org/SDL/blob/main/docs/README-migration.md).

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

## additional changes

- clean up `CMakeLists.txt`
- update installation files + documentation to SDL3
    - `Brewfile`
    - `install-RPI.sh`
    - `DOCUMENTATION.md`
    - `README.md`
