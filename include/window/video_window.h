#pragma once
#include <cstdint>
#include <window/sdl_window.h>
#include <window/sdl_renderer.h>
#include <memory>
#include <capture/avcodec.h>
extern "C" {
#include <libavcodec/avcodec.h>
}

class video_window : public sdl_window {
public:
    video_window(const std::string& name, uint32_t width, uint32_t height);

    void handle_frame(int64_t pts, int64_t dts, int32_t flags, uint8_t* data, uint64_t sz);

    ~video_window() = default;

public:
    avcodec decoder;

    uint32_t width;
    uint32_t height;

    std::unique_ptr<sdl_renderer> renderer;
    std::unique_ptr<sdl_texture> renderTex;
};
