#pragma once
#include <memory>
#include <window/sdl_window.h>
#include <SDL2/SDL.h>
#include <window/sdl_texture.h>

class sdl_renderer {
public:
    sdl_renderer(sdl_window* window, int32_t index, int32_t flags);

    std::unique_ptr<sdl_texture> createTexture(uint32_t format, int32_t access, uint32_t width, uint32_t height);

    void setRendertarget(std::unique_ptr<sdl_texture> tex);
    void renderCopy(SDL_Texture* tex, const SDL_Rect* srcrect, const SDL_Rect* dstrect);
    void renderPresent();

    ~sdl_renderer();
public:
    SDL_Renderer* renderer;
};
