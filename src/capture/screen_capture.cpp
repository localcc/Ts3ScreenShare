//
// Created by localcc on 3/20/21.
//
#include <capture/screen_capture.h>
#include <iostream>
#include <network/packets/StartStreamPacket.h>
#include <network/packets/StopStreamPacket.h>

screen_capture::screen_capture(std::shared_ptr<udp_client> client, int32_t width, int32_t height) :
    running(false),
    client(std::move(client)),
    input(":0.0", "x11grab", {{"video_size", std::to_string(width) + "x" + std::to_string(height)}}),
    decoder(input.get_best_stream(AVMEDIA_TYPE_VIDEO, -1, -1, 0).has_value() ?
                       input.get_best_stream(AVMEDIA_TYPE_VIDEO, -1, -1, 0).value()->codecpar : throw std::runtime_error("s")),
    encoder(AV_CODEC_ID_H264, AV_PIX_FMT_YUV420P, width, height, {{"tune", "zerolatency", 0}, {"preset", "ultrafast", 0}}, true),
    scalingContext(width, height, decoder.getCodecContext()->pix_fmt, width, height, AV_PIX_FMT_YUV420P, SWS_BICUBIC)
    {
}

int32_t screen_capture::start() {
    int32_t res = this->decoder.open();
    if(res < 0) {
        std::cout << "Failed to open decoder!" << std::endl;
        return res;
    }

    this->encoder.getCodecContext()->time_base.den = 1; // fps
    this->encoder.getCodecContext()->time_base.num = 60; // todo: load from config
    this->encoder.getCodecContext()->bit_rate = 1000 * 1000;
    this->encoder.getCodecContext()->max_b_frames = 0;
    this->encoder.getCodecContext()->thread_count = 0;
    this->encoder.getCodecContext()->gop_size = 250;


    res = this->encoder.open();
    if(res < 0) {
        std::cout << "Failed to open encoder!" << std::endl;
        return res;
    }

    if(this->running) {
        return 0;
    }

    if(!this->client->isConnected()) {
        std::cout << "Client not connected!" << std::endl;
        return -1;
    }

    this->running = true;

    StartStreamPacket packet{
                .width = this->decoder.getCodecContext()->width,
                .height = this->decoder.getCodecContext()->height,
                .pixFmt = this->decoder.getCodecContext()->pix_fmt};
    this->client->sock_write(reinterpret_cast<uint8_t*>(&packet), sizeof(StartStreamPacket));

    this->captureThread = std::thread([&](){
        AVPacket* packet = av_packet_alloc();
        AVFrame* frame = av_frame_alloc();
        AVFrame* colorFrame = av_frame_alloc();

        AVPacket* encodedPacket = av_packet_alloc();

        auto* formatContext = this->input.getFormatContext();
        auto* decoderContext = this->decoder.getCodecContext();
        auto* encoderContext = this->encoder.getCodecContext();

        while(av_read_frame(formatContext, packet) > -1 && this->running) {
            int32_t res = avcodec_send_packet(decoderContext, packet);
            if(res < 0) {
                //todo: handle codes
            }
            res = avcodec_receive_frame(decoderContext, frame);
            if(res < 0) {
                //todo: handle codes
            }
            av_packet_unref(packet);


            av_image_alloc(colorFrame->data, colorFrame->linesize, decoderContext->width, decoderContext->height, AV_PIX_FMT_YUV420P, 1);
            colorFrame->width = frame->width;
            colorFrame->height = frame->height;
            colorFrame->pts = frame->pts;
            colorFrame->format = AV_PIX_FMT_YUV420P;

            auto* const converterContext = this->scalingContext.get();
            sws_scale(converterContext, frame->data, frame->linesize, 0, frame->height, colorFrame->data, colorFrame->linesize);
            av_frame_unref(frame);


            res = avcodec_send_frame(encoderContext, colorFrame);
            if(res < 0) {
                if(res == AVERROR(EAGAIN)) {
                    std::cout << "Eagain" << std::endl;
                    av_freep(&colorFrame->data[0]);
                    av_frame_unref(colorFrame);
                    continue;
                }
                //todo: handle codes
                std::cout << "Failed to send: " << res << std::endl;
            }

            res = avcodec_receive_packet(encoderContext, encodedPacket);
            if(res < 0) {
                if(res == AVERROR(EAGAIN)) {
                    std::cout << "Eagain" << std::endl;
                    av_freep(&colorFrame->data[0]);
                    av_frame_unref(colorFrame);
                    continue;
                }
                //todo: handle codes
                std::cout << "Failed to receive: " << res << std::endl;
            }
            av_freep(&colorFrame->data[0]);
            av_frame_unref(colorFrame);


            auto* flatFramePacket = new FlatFramePacket{.pts = encodedPacket->pts, .dts = encodedPacket->dts, .flags = encodedPacket->flags};

            auto* const data = new uint8_t[encodedPacket->size + sizeof(FlatFramePacket)]; // size + pts size + dts size + flags size + header

            std::move(reinterpret_cast<uint8_t*>(flatFramePacket), reinterpret_cast<uint8_t*>(flatFramePacket) + sizeof(FlatFramePacket), data);
            delete flatFramePacket;

            std::copy(encodedPacket->data, encodedPacket->data + encodedPacket->size, data + sizeof(FlatFramePacket));

            this->client->sock_write(data, encodedPacket->size + sizeof(FlatFramePacket));

            delete[] data;

            av_packet_unref(encodedPacket);
        }

        av_packet_free(&encodedPacket);
        av_frame_free(&colorFrame);
        av_frame_free(&frame);
        av_packet_free(&packet);
    });

    return 0;
}

void screen_capture::stop() {
    StopStreamPacket packet{};
    this->client->sock_write(reinterpret_cast<uint8_t*>(&packet), sizeof(StopStreamPacket));
    if(this->running) {
        this->running = false;
        this->captureThread.join();
    }
}
