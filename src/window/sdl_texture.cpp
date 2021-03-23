#include <window/sdl_texture.h>

sdl_texture::sdl_texture(SDL_Texture* tex) {
    this->tex = tex;
}

sdl_texture::~sdl_texture() {
    SDL_DestroyTexture(this->tex);
}


int32_t sdl_texture::lock(const SDL_Rect* rect, void** pixels, int32_t* pitch) {
    return SDL_LockTexture(this->tex, rect, pixels, pitch);
}

void sdl_texture::unlock() {
    SDL_UnlockTexture(this->tex);
}
