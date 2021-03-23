#include <window/sdl_window.h>

sdl_window::sdl_window(const std::string& name, int32_t posX, int32_t posY, uint32_t width, uint32_t height, int32_t flags) {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error("Failed to init sdl!");
    }

    //this->window = SDL_CreateWindow(name.c_str(), posX, posY, width, height, flags);
    this->window = SDL_CreateWindow("suka", 0, 0, 800, 600, SDL_WINDOW_RESIZABLE);
    if(window == nullptr) {
        throw std::runtime_error("Failed to init window!");
    }
}

sdl_window::~sdl_window() {
    SDL_DestroyWindow(this->window);
}
