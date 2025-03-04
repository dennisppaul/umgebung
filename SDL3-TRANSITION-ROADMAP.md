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

- [ ] implement atlas-based font rendering ( with FreeType )
- [ ] add *immediate* mode
- [ ] add dedicated bins for transparent and non-transparent primitives in *retained* mode
- [ ] MSAA FBOs
    - [ ] implement line width

#### MSAA FBOs

Modify FBO Setup for MSAA:

```C
GLuint fbo, msaaColorBuffer, msaaDepthBuffer;
const int samples = 4; // Number of MSAA samples

// Create FBO
glGenFramebuffers(1, &fbo);
glBindFramebuffer(GL_FRAMEBUFFER, fbo);

// Create Multisampled Color Buffer
glGenTextures(1, &msaaColorBuffer);
glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaaColorBuffer);
glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA8, width, height, GL_TRUE);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaaColorBuffer, 0);

// Create Multisampled Depth Buffer
glGenRenderbuffers(1, &msaaDepthBuffer);
glBindRenderbuffer(GL_RENDERBUFFER, msaaDepthBuffer);
glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, msaaDepthBuffer);

if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "MSAA FBO is incomplete!" << std::endl;
}

glBindFramebuffer(GL_FRAMEBUFFER, 0);
```

Render VBOs Into the MSAA FBO:

```C
glBindFramebuffer(GL_FRAMEBUFFER, fbo);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// Render VBOs here...

glBindFramebuffer(GL_FRAMEBUFFER, 0);
```

Resolve the Multisampled FBO into a Regular Texture:

```C
glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screenFBO); // Regular FBO to store resolved image
glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
glBindFramebuffer(GL_FRAMEBUFFER, 0);
```

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