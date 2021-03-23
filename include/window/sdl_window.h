#pragma once
#include <string>
#include <SDL2/SDL.h>
#include <stdexcept>

class sdl_renderer;

class sdl_window {
public:
    sdl_window(const std::string& name, int32_t posX, int32_t posY, uint32_t width, uint32_t height, int32_t flags);

    ~sdl_window();
private:
    friend class sdl_renderer;

    SDL_Window* window;
};
