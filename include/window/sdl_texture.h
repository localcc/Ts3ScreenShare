#pragma once
#include <SDL2/SDL.h>

class sdl_texture {
public:
    sdl_texture(SDL_Texture* texture);

    int32_t lock(const SDL_Rect* rect, void** pixels, int32_t* pitch);
    void unlock();

    ~sdl_texture();

public:

    SDL_Texture* tex;
};
