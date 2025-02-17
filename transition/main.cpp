#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/* ------------------------------------------------------------------------------------ */

typedef struct Context {
    const char*    ExampleName;
    const char*    BasePath;
    SDL_Window*    Window;
    SDL_GPUDevice* Device;
    float          DeltaTime;
} Context;

int CommonInit(Context* context, SDL_WindowFlags windowFlags) {
    context->Device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
        false,
        NULL);

    if (context->Device == NULL) {
        SDL_Log("GPUCreateDevice failed");
        return -1;
    }

    context->Window = SDL_CreateWindow(context->ExampleName, 640, 480, windowFlags);
    if (context->Window == NULL) {
        SDL_Log("CreateWindow failed: %s", SDL_GetError());
        return -1;
    }

    if (!SDL_ClaimWindowForGPUDevice(context->Device, context->Window)) {
        SDL_Log("GPUClaimWindow failed");
        return -1;
    }

    return 0;
}

void CommonQuit(Context* context) {
    SDL_ReleaseWindowFromGPUDevice(context->Device, context->Window);
    SDL_DestroyWindow(context->Window);
    SDL_DestroyGPUDevice(context->Device);
}

static const char* BasePath = NULL;
void               InitializeAssetLoader() {
    BasePath = SDL_GetBasePath();
}

SDL_GPUShader* LoadShader(
    SDL_GPUDevice* device,
    const char*    shaderFilename,
    Uint32         samplerCount,
    Uint32         uniformBufferCount,
    Uint32         storageBufferCount,
    Uint32         storageTextureCount) {
    // Auto-detect the shader stage from the file name for convenience
    SDL_GPUShaderStage stage;
    if (SDL_strstr(shaderFilename, ".vert")) {
        stage = SDL_GPU_SHADERSTAGE_VERTEX;
    } else if (SDL_strstr(shaderFilename, ".frag")) {
        stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    } else {
        SDL_Log("Invalid shader stage!");
        return NULL;
    }

    char                fullPath[256];
    SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
    SDL_GPUShaderFormat format         = SDL_GPU_SHADERFORMAT_INVALID;
    const char*         entrypoint;

    if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
        SDL_Log("shader format: SPIRV");
        SDL_snprintf(fullPath, sizeof(fullPath), "%s../Shaders/Compiled/SPIRV/%s.spv", BasePath, shaderFilename);
        format     = SDL_GPU_SHADERFORMAT_SPIRV;
        entrypoint = "main";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
        SDL_Log("shader format: MSL");
        SDL_snprintf(fullPath, sizeof(fullPath), "%s../Shaders/Compiled/MSL/%s.msl", BasePath, shaderFilename);
        format     = SDL_GPU_SHADERFORMAT_MSL;
        entrypoint = "main0";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
        SDL_Log("shader format: DXIL");
        SDL_snprintf(fullPath, sizeof(fullPath), "%s../Shaders/Compiled/DXIL/%s.dxil", BasePath, shaderFilename);
        format     = SDL_GPU_SHADERFORMAT_DXIL;
        entrypoint = "main";
    } else {
        SDL_Log("%s", "Unrecognized backend shader format!");
        return NULL;
    }

    size_t codeSize;
    void*  code = SDL_LoadFile(fullPath, &codeSize);
    if (code == NULL) {
        SDL_Log("Failed to load shader from disk! %s", fullPath);
        return NULL;
    }

    SDL_GPUShaderCreateInfo shaderInfo = {
        .code                 = (const Uint8*) code,
        .code_size            = codeSize,
        .entrypoint           = entrypoint,
        .format               = format,
        .stage                = stage,
        .num_samplers         = samplerCount,
        .num_uniform_buffers  = uniformBufferCount,
        .num_storage_buffers  = storageBufferCount,
        .num_storage_textures = storageTextureCount};
    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
    if (shader == NULL) {
        SDL_Log("Failed to create shader!");
        SDL_free(code);
        return NULL;
    }

    SDL_free(code);
    return shader;
}

/* ------------------------------------------------------------------------------------ */

static SDL_GPUGraphicsPipeline* FillPipeline;
static SDL_GPUGraphicsPipeline* LinePipeline;
static SDL_GPUViewport          SmallViewport = {160, 120, 320, 240, 0.1f, 1.0f};
static SDL_Rect                 ScissorRect   = {320, 240, 320, 240};

static bool UseWireframeMode = false;
static bool UseSmallViewport = false;
static bool UseScissorRect   = false;

static int Init(Context* context) {
    int result = CommonInit(context, 0);
    if (result < 0) {
        return result;
    }

    // Create the shaders
    SDL_GPUShader* vertexShader = LoadShader(context->Device, "RawTriangle.vert", 0, 0, 0, 0);
    if (vertexShader == NULL) {
        SDL_Log("Failed to create vertex shader!");
        return -1;
    }

    SDL_GPUShader* fragmentShader = LoadShader(context->Device, "SolidColor.frag", 0, 0, 0, 0);
    if (fragmentShader == NULL) {
        SDL_Log("Failed to create fragment shader!");
        return -1;
    }

    // Create the pipelines
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .target_info = {
            .num_color_targets         = 1,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]) {{.format = SDL_GetGPUSwapchainTextureFormat(context->Device, context->Window)}},
        },
        .primitive_type  = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader   = vertexShader,
        .fragment_shader = fragmentShader,
    };

    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    FillPipeline                                  = SDL_CreateGPUGraphicsPipeline(context->Device, &pipelineCreateInfo);
    if (FillPipeline == NULL) {
        SDL_Log("Failed to create fill pipeline!");
        return -1;
    }

    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_LINE;
    LinePipeline                                  = SDL_CreateGPUGraphicsPipeline(context->Device, &pipelineCreateInfo);
    if (LinePipeline == NULL) {
        SDL_Log("Failed to create line pipeline!");
        return -1;
    }

    // Clean up shader resources
    SDL_ReleaseGPUShader(context->Device, vertexShader);
    SDL_ReleaseGPUShader(context->Device, fragmentShader);

    // Finally, print instructions!
    // SDL_Log("Press Left to toggle wireframe mode");
    // SDL_Log("Press Down to toggle small viewport");
    // SDL_Log("Press Right to toggle scissor rect");

    return 0;
}

static int Update(Context* context) {
    // 	if (context->LeftPressed)
    // 	{
    // 		UseWireframeMode = !UseWireframeMode;
    // 	}
    //
    // 	if (context->DownPressed)
    // 	{
    // 		UseSmallViewport = !UseSmallViewport;
    // 	}
    //
    // 	if (context->RightPressed)
    // 	{
    // 		UseScissorRect = !UseScissorRect;
    // 	}

    return 0;
}

static int Draw(Context* context) {
    SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(context->Device);
    if (cmdbuf == NULL) {
        SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return -1;
    }

    SDL_GPUTexture* swapchainTexture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdbuf, context->Window, &swapchainTexture, NULL, NULL)) {
        SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        return -1;
    }

    if (swapchainTexture != NULL) {
        SDL_GPUColorTargetInfo colorTargetInfo = {0};
        colorTargetInfo.texture                = swapchainTexture;
        colorTargetInfo.clear_color            = (SDL_FColor) {0.0f, 0.0f, 0.0f, 1.0f};
        colorTargetInfo.load_op                = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op               = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, NULL);
        SDL_BindGPUGraphicsPipeline(renderPass, UseWireframeMode ? LinePipeline : FillPipeline);
        if (UseSmallViewport) {
            SDL_SetGPUViewport(renderPass, &SmallViewport);
        }
        if (UseScissorRect) {
            SDL_SetGPUScissor(renderPass, &ScissorRect);
        }
        SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
        SDL_EndGPURenderPass(renderPass);
    }

    SDL_SubmitGPUCommandBuffer(cmdbuf);

    return 0;
}

static void Quit(Context* context) {
    SDL_ReleaseGPUGraphicsPipeline(context->Device, FillPipeline);
    SDL_ReleaseGPUGraphicsPipeline(context->Device, LinePipeline);

    UseWireframeMode = false;
    UseSmallViewport = false;
    UseScissorRect   = false;

    CommonQuit(context);
}

// Example BasicTriangle_Example = { "BasicTriangle", Init, Update, Draw, Quit };

/* ------------------------------------------------------------------------------------ */

bool AppLifecycleWatcher(void* userdata, SDL_Event* event) {
    /* This callback may be on a different thread, so let's
	 * push these events as USER events so they appear
	 * in the main thread's event loop.
	 *
	 * That allows us to cancel drawing before/after we finish
	 * drawing a frame, rather than mid-draw (which can crash!).
	 */
    if (event->type == SDL_EVENT_DID_ENTER_BACKGROUND) {
        SDL_Event evt;
        evt.type      = SDL_EVENT_USER;
        evt.user.code = 0;
        SDL_PushEvent(&evt);
    } else if (event->type == SDL_EVENT_WILL_ENTER_FOREGROUND) {
        SDL_Event evt;
        evt.type      = SDL_EVENT_USER;
        evt.user.code = 1;
        SDL_PushEvent(&evt);
    }
    return false;
}

int main(const int argc, char** argv) {
    Context context  = {0};
    int     quit     = 0;
    float   lastTime = 0;

    for (int i = 1; i < argc; i += 1) {
        if (SDL_strcmp(argv[i], "-name") == 0 && argc > i + 1) {
            const char* mName = argv[i + 1];
            SDL_Log("name: '%s'", mName);
        }
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    InitializeAssetLoader();
    Init(&context);
    SDL_AddEventWatch(AppLifecycleWatcher, NULL);

    SDL_Gamepad* gamepad = NULL;
    bool         canDraw = true;

    while (!quit) {
        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_EVENT_QUIT) {
                // 				if (exampleIndex != -1) {
                // 					Examples[exampleIndex]->Quit(&context);
                // 				}
                SDL_Log("quit");
                Quit(&context);
                quit = 1;
            } else if (evt.type == SDL_EVENT_GAMEPAD_ADDED) {
                if (gamepad == NULL) {
                    gamepad = SDL_OpenGamepad(evt.gdevice.which);
                }
            } else if (evt.type == SDL_EVENT_GAMEPAD_REMOVED) {
                if (evt.gdevice.which == SDL_GetGamepadID(gamepad)) {
                    SDL_CloseGamepad(gamepad);
                }
            } else if (evt.type == SDL_EVENT_USER) {
                if (evt.user.code == 0) {
#ifdef SDL_PLATFORM_GDK
                    SDL_GDKSuspendGPU(context.Device);
                    canDraw = false;
                    SDL_GDKSuspendComplete();
#endif
                } else if (evt.user.code == 1) {
#ifdef SDL_PLATFORM_GDK
                    SDL_GDKResumeGPU(context.Device);
                    canDraw = true;
#endif
                }
            } else if (evt.type == SDL_EVENT_KEY_DOWN) {
                if (evt.key.key == SDLK_D) {
                    // 					gotoExampleIndex = exampleIndex + 1;
                    // 					if (gotoExampleIndex >= SDL_arraysize(Examples)) {
                    // 						gotoExampleIndex = 0;
                    // 					}
                    SDL_Log("Pressed D");
                }
                if (evt.key.key == SDLK_ESCAPE) {
                    SDL_Log("quit");
                    Quit(&context);
                    quit = 1;
                }
            } else if (evt.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
                SDL_Log("SDL_EVENT_GAMEPAD_BUTTON_DOWN");
                // 				if (evt.gbutton.button == SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)
                // 				{
                // 					gotoExampleIndex = exampleIndex + 1;
                // 					if (gotoExampleIndex >= SDL_arraysize(Examples)) {
                // 						gotoExampleIndex = 0;
                // 					}
                // 				}
                // 				else if (evt.gbutton.button == SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)
                // 				{
                // 					gotoExampleIndex = exampleIndex - 1;
                // 					if (gotoExampleIndex < 0) {
                // 						gotoExampleIndex = SDL_arraysize(Examples) - 1;
                // 					}
                // 				}
                // 				else if (evt.gbutton.button == SDL_GAMEPAD_BUTTON_DPAD_LEFT)
                // 				{
                // 					context.LeftPressed = true;
                // 				}
                // 				else if (evt.gbutton.button == SDL_GAMEPAD_BUTTON_DPAD_RIGHT)
                // 				{
                // 					context.RightPressed = true;
                // 				}
                // 				else if (evt.gbutton.button == SDL_GAMEPAD_BUTTON_DPAD_DOWN)
                // 				{
                // 					context.DownPressed = true;
                // 				}
                // 				else if (evt.gbutton.button == SDL_GAMEPAD_BUTTON_DPAD_UP)
                // 				{
                // 					context.UpPressed = true;
                // 				}
            }
        }
        if (quit) {
            break;
        }

        // 		if (gotoExampleIndex != -1)
        // 		{
        // 			if (exampleIndex != -1)
        // 			{
        // 				Examples[exampleIndex]->Quit(&context);
        // 				SDL_zero(context);
        // 			}
        //
        // 			exampleIndex = gotoExampleIndex;
        // 			context.ExampleName = Examples[exampleIndex]->Name;
        // 			SDL_Log("STARTING EXAMPLE: %s", context.ExampleName);
        // 			if (Examples[exampleIndex]->Init(&context) < 0)
        // 			{
        // 				SDL_Log("Init failed!");
        // 				return 1;
        // 			}
        //
        // 			gotoExampleIndex = -1;
        // 		}

        float newTime     = SDL_GetTicks() / 1000.0f;
        context.DeltaTime = newTime - lastTime;
        lastTime          = newTime;

        if (Update(&context) < 0) {
            SDL_Log("Update failed!");
            return 1;
        }

        if (canDraw) {
            if (Draw(&context) < 0) {
                SDL_Log("Draw failed!");
                return 1;
            }
        }
    }

    return 0;
}
