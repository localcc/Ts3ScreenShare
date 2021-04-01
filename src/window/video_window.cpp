#include <window/video_window.h>
#include <network/NetworkHelper.h>
#include <iostream>
#include <chrono>

video_window::video_window(const std::string& name, uint32_t width, uint32_t height) :
        sdl_window(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE),
        decoder(AV_CODEC_ID_H264, AV_PIX_FMT_YUV420P, width, height),
        srcWidth(width), srcHeight(height),
        scaledWidth(width), scaledHeight(height),
        textureMoveRect{.x = 0, .y = 0, .w = static_cast<int32_t>(width), .h = static_cast<int32_t>(height)},
        srcAspectRatio(static_cast<float>(width) / static_cast<float>(height)),
        srcInverseAspectRatio(static_cast<float>(height) / static_cast<float>(width)),
        renderer(std::make_unique<sdl_renderer>(this, -1, SDL_RENDERER_ACCELERATED)),
        renderTex(renderer->createTexture(SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width, height)),
        scalingCtx(std::make_unique<swscontext>(width, height, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_YUV420P, SWS_BICUBIC))
{
    const auto res = decoder.open();
    if(res < 0) {
        throw std::runtime_error("Failed to open codec: " + std::to_string(res));
    }
}

void video_window::handle_frame(int64_t pts, int64_t dts, int32_t flags, uint8_t *data, uint64_t sz)
{
    std::cout << "Handling frame!" << std::endl;
    AVPacket* packet = av_packet_alloc();

    av_packet_from_data(packet, data, sz);
    packet->pts = pts;
    packet->dts = dts;
    packet->flags = flags;


    auto* const codecContext = this->decoder.getCodecContext();
    int32_t res = avcodec_send_packet(codecContext, packet);
    if(res < 0) {
        //FIXME: handle codes
        if(res != AVERROR(EAGAIN)) return;
    }

    AVFrame* frame = av_frame_alloc();

    res = avcodec_receive_frame(codecContext, frame);
    if(res < 0) {
        //FIXME: handle codes
        if(res == AVERROR(EAGAIN)) {
            av_frame_unref(frame);
            av_frame_free(&frame);

            av_packet_unref(packet);
            av_packet_free(&packet);
            return;
        }
        return;
    }

    AVFrame* rescaledFrame = av_frame_alloc();
    av_image_alloc(rescaledFrame->data, rescaledFrame->linesize, this->scaledWidth, this->scaledHeight, AV_PIX_FMT_YUV420P, 16);
    sws_scale(this->scalingCtx->get(), frame->data, frame->linesize, 0, frame->height, rescaledFrame->data, rescaledFrame->linesize);

    av_frame_unref(frame);
    av_frame_free(&frame);

    SDL_UpdateYUVTexture(this->renderTex->tex, nullptr, rescaledFrame->data[0], rescaledFrame->linesize[0], rescaledFrame->data[1], rescaledFrame->linesize[1], rescaledFrame->data[2], rescaledFrame->linesize[2]);
    av_freep(&rescaledFrame->data[0]);
    av_frame_unref(rescaledFrame);


    SDL_SetRenderTarget(this->renderer->renderer, nullptr);
    SDL_RenderClear(this->renderer->renderer);
    SDL_RenderCopy(this->renderer->renderer, this->renderTex->tex, nullptr, &this->textureMoveRect);
    SDL_RenderPresent(this->renderer->renderer);


    SDL_Event event;


    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_WINDOWEVENT) {
            switch(event.window.event) {
                //FIXME: add rescaling handling
                case SDL_WINDOWEVENT_CLOSE: {
                    if(event.window.windowID == SDL_GetWindowID(this->window)) {
                        std::cout << "Quitting!" << std::endl;
                        this->closing = true;
                    }
                    break;
                }
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    this->scaledWidth = event.window.data1;
                    this->scaledHeight = event.window.data2;

                    std::cout << this->srcWidth << " " << this->srcHeight << " " << this->scaledWidth << " " << this->scaledHeight << std::endl;

                    float scaledAspectRatio = this->scaledWidth / static_cast<float>(this->scaledHeight);

                    if(scaledAspectRatio > this->srcAspectRatio) {
                        std::cout << "Changing width!" << std::endl;
                        this->scaledWidth = this->srcAspectRatio * this->scaledHeight;
                    }

                    if(scaledAspectRatio < this->srcAspectRatio) {
                        std::cout << "Changing height!" << std::endl;
                        this->scaledHeight = this->srcInverseAspectRatio * this->scaledWidth;
                    }
                    if(this->scaledHeight == 0) {
                        this->scaledHeight = 1;
                    }
                    if(this->scaledWidth == 0) {
                        this->scaledWidth = 1;
                    }


                    std::cout << this->scaledWidth << " " << this->scaledHeight << " " << this->srcAspectRatio << std::endl;

                    int32_t horizontalMove = (event.window.data1 - this->scaledWidth) / 2 ;
                    int32_t verticalMove = (event.window.data2 - this->scaledHeight) / 2;
                    this->textureMoveRect.x = horizontalMove;
                    this->textureMoveRect.y = verticalMove;
                    this->textureMoveRect.w = this->scaledWidth;
                    this->textureMoveRect.h = this->scaledHeight;
                    std::cout << "X: " << this->textureMoveRect.x << " Y:" << this->textureMoveRect.y << " W: " << this->textureMoveRect.w << " H: " << this->textureMoveRect.h << std::endl;


                    this->renderTex.reset();
                    this->renderTex = std::unique_ptr<sdl_texture>(this->renderer->createTexture(SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, this->scaledWidth, this->scaledHeight));
                    this->scalingCtx->update(this->srcWidth, this->srcHeight, AV_PIX_FMT_YUV420P, this->scaledWidth, this->scaledHeight, AV_PIX_FMT_YUV420P, SWS_BICUBIC);
                    break;
                }
            }
        }
    }

    /*
     *
    memcpy(pixels1,             pictureBufferY, size1  );
    memcpy(pixels1 + size1,     pictureBufferV, size1/4);
    memcpy(pixels1 + size1*5/4, pictureBufferU, size1/4);*/
}

