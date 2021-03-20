//
// Created by localcc on 3/20/21.
//

#pragma once
#include <cstdint>
#include <network/udp_client.h>
#include <capture/avcodec.h>
#include <capture/avinput.h>
extern "C" {
#include <libavformat/avformat.h>
}
#include <string>
#include <memory>

class screen_capture {
public:
    screen_capture(std::shared_ptr<udp_client> client, int32_t width, int32_t height);

    int32_t start();
    void stop();

    ~screen_capture() = default;
private:
    bool running;

    std::shared_ptr<udp_client> client;
    avinput input;
    avcodec codec;
};