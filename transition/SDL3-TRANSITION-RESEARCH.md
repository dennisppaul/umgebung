# SDL3-TRANSITION-RESEARCH

## SDL_gpu

- https://github.com/TheSpydog/SDL_gpu_examples
- https://github.com/libsdl-org/SDL_shadercross.git ( requires [SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross) to be compiled and installed )

### compiling HLSL into specific GPU device shader languages

`SDL_GetGPUShaderFormats(SDL_GPUDevice *device)` in SDL3’s GPU API returns a bitflag indicating which shader formats the specified GPU device can consume. different graphics backends require shaders in specific formats:

- Vulkan :: Utilizes SPIR-V (Standard Portable Intermediate Representation - Vulkan) binaries.
- Direct3D 12 :: Requires DXIL (DirectX Intermediate Language) or DXBC (DirectX Bytecode) shaders.
- Metal :: Employs MSL (Metal Shading Language) binaries.

for macOS and iOS, you’ll need to provide shaders in Metal Shading Language (MSL) format, as SDL3’s GPU API utilizes Metal as the underlying graphics backend on these platforms. For Linux systems, including the Raspberry Pi, Vulkan is commonly used, which requires shaders in SPIR-V format.

To streamline shader management across these platforms, consider using SDL_shadercross, a tool designed to translate shaders between different formats compatible with SDL’s GPU API. This allows you to write shaders once, typically in a high-level language like HLSL or GLSL, and then convert them to the necessary formats (e.g., MSL for Metal, SPIR-V for Vulkan) as part of your build process. This approach simplifies development and ensures compatibility across multiple platforms.

By leveraging SDL_shadercross, you can maintain a unified shader codebase, reducing the complexity associated with supporting multiple graphics backends in your cross-platform application.

as a future improvement shadercross could be executed at runtime:

```C
#include <SDL3/SDL.h>
#include <SDL3/SDL_shadercross.h>

// Assume 'device' is your initialized SDL_GPUDevice*
// 'hlslSource' contains your HLSL shader code as a string

SDL_GPUShader* vertexShader = SDL_ShaderCross_CompileGraphicsShaderFromHLSL(
    device,
    hlslSource,
    "main",  // Entry point
    NULL,    // Optional include callback
    NULL,    // Optional include user data
    0,       // Flags
    SDL_GPU_SHADERSTAGE_VERTEX,
    NULL     // Optional pointer to receive shader info
);

if (!vertexShader) {
    // Handle compilation error
}
```
