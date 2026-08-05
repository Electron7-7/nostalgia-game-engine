#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"
#include "SDL3/SDL.h"

SDL_Window* window;
SDL_GPUDevice* device;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
    window = SDL_CreateWindow("Hello Triangle!", 960, 540, SDL_WINDOW_RESIZABLE);
    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, nullptr);
    SDL_ClaimWindowForGPUDevice(device, window);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{ return SDL_APP_CONTINUE; }

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if(event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        { return SDL_APP_SUCCESS; }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);
}

void say_hello_world()
{
    std::println("Hello World!");

}
