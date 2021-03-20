//
// Created by localcc on 3/20/21.
//
#include <capture/screen_capture.h>

screen_capture::screen_capture(std::shared_ptr<udp_client> client, int32_t width, int32_t height) :
    running(false),
    client(std::move(client)),
    input(":0.0", "x11grab", {{"video_size", std::to_string(width) + "x" + std::to_string(height)}}),
    codec(input.get_best_stream(AVMEDIA_TYPE_VIDEO, -1, -1, 0).has_value() ?
                       input.get_best_stream(AVMEDIA_TYPE_VIDEO, -1, -1, 0).value()->codecpar : throw std::runtime_error("s"))
    {
}

int32_t screen_capture::start() {
    int32_t res = this->codec.open();
    if(res < 0) {
        return res;
    }

    this->running = true;

    AVFrame* frame = av_frame_alloc();
    AVPacket* packet = av_packet_alloc();

    auto* formatContext = this->input.getFormatContext();
    auto* codecContext = this->codec.getCodecContext();

    auto* const dummyData = new uint8_t[10];


    while(av_read_frame(formatContext, packet) > -1 && this->running) {
        res = avcodec_send_packet(codecContext, packet);
        if(res == AVERROR(EAGAIN)) {
            continue;
        }
        //todo: handle other results differently but i don't know how for now

        av_packet_unref(packet);

        res = avcodec_receive_frame(codecContext, frame);
        if(res == AVERROR(EAGAIN)) {
            continue;
        }
        //todo: handle other results differently but i don't know how for now

        this->client->sock_write(dummyData, 10);
    }
    av_packet_free(&packet);
    av_frame_free(&frame);

    return 0;
}

void screen_capture::stop() {
    //todo: notify server about stopping
    this->running = false;
}