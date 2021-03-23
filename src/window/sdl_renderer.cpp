#include <window/sdl_renderer.h>


sdl_renderer::sdl_renderer(sdl_window* window, int32_t index, int32_t flags)
{
    this->renderer = SDL_CreateRenderer(window->window, index, flags);
}

std::unique_ptr<sdl_texture> sdl_renderer::createTexture(uint32_t format, int32_t access, uint32_t width, uint32_t height)
{
    return std::make_unique<sdl_texture>(SDL_CreateTexture(this->renderer, format, access, width, height));
}

void sdl_renderer::setRendertarget(std::unique_ptr<sdl_texture> tex)
{
    SDL_SetRenderTarget(this->renderer, tex->tex);
}

void sdl_renderer::renderCopy(SDL_Texture* tex, const SDL_Rect *srcrect, const SDL_Rect *dstrect)
{
    SDL_RenderCopy(this->renderer, tex, srcrect, dstrect);
}

void sdl_renderer::renderPresent()
{
    SDL_RenderPresent(this->renderer);
}

sdl_renderer::~sdl_renderer()
{
    SDL_DestroyRenderer(this->renderer);
}
