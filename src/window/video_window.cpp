#include <window/video_window.h>
#include <network/NetworkHelper.h>
#include <iostream>

video_window::video_window(const std::string& name, uint32_t width, uint32_t height) :
    sdl_window(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE),
    decoder(AV_CODEC_ID_H264, AV_PIX_FMT_YUV420P, width, height),
    width(width), height(height),
    renderer(std::make_unique<sdl_renderer>(this, -1, SDL_RENDERER_ACCELERATED)),
    renderTex(renderer->createTexture(SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width, height))
{
    const auto res = decoder.open();
    if(res < 0) {
        throw std::runtime_error("Failed to open coddec: " + std::to_string(res));
    }
}

void video_window::handle_frame(int64_t pts, int64_t dts, int32_t flags, uint8_t *data, uint64_t sz)
{
    AVPacket* packet = av_packet_alloc();

    av_packet_from_data(packet, data + 21, sz - 21);
    packet->pts = pts;
    packet->dts = dts;
    packet->flags = flags;


    auto* const codecContext = this->decoder.getCodecContext();
    int32_t res = avcodec_send_packet(codecContext, packet);
    if(res < 0) {
        if(res == AVERROR(EAGAIN)) {
            av_packet_unref(packet);
            av_packet_free(&packet);

            std::cout << "Send eagain" << std::endl;
            return;
        }
    }
    av_packet_unref(packet);
    av_packet_free(&packet);

    AVFrame* frame = av_frame_alloc();

    res = avcodec_receive_frame(codecContext, frame);
    if(res < 0) {
        if(res == AVERROR(EAGAIN)) {
            av_frame_unref(frame);
            av_frame_free(&frame);

            std::cout << "Eagain" << std::endl;
            return;
        }
    }
    av_frame_unref(frame);
    av_frame_free(&frame);

    const auto size = this->width * this->height;
    void* pixels;
    int pitch;
    if(this->renderTex->lock(nullptr, &pixels, &pitch) != 0) {
        std::cout << "Failed to lock texture: " << SDL_GetError() << std::endl;
        return;
    }
    std::copy(frame->data[0], frame->data[0] + size, reinterpret_cast<uint8_t*>(pixels));
    std::copy(frame->data[2], frame->data[2] + (size/4), reinterpret_cast<uint8_t*>(pixels) + size);
    std::copy(frame->data[1], frame->data[1] + (size/4), reinterpret_cast<uint8_t*>(pixels) + (size*5/4));
    this->renderTex->unlock();

    SDL_SetRenderTarget(this->renderer->renderer, nullptr);
    SDL_RenderClear(this->renderer->renderer);
    SDL_RenderCopy(this->renderer->renderer, this->renderTex->tex, nullptr, nullptr);
    SDL_RenderPresent(this->renderer->renderer);
    std::cout << "Finalized rendering!" << std::endl;
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        std::cout << "Event type: " << event.type << std::endl;
    }
    /*
     *
    memcpy(pixels1,             pictureBufferY, size1  );
    memcpy(pixels1 + size1,     pictureBufferV, size1/4);
    memcpy(pixels1 + size1*5/4, pictureBufferU, size1/4);*/
}

