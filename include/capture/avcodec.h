//
// Created by localcc on 3/20/21.
//

#pragma once
#include <cstdint>
#include <stdexcept>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}
#include <vector>
#include <tuple>

class avcodec {
public:
    explicit avcodec(AVCodecParameters* codecParameters, std::vector<std::tuple<std::string, std::string, int32_t>> opts = {}, bool encoder = false);
    avcodec(AVCodecID id, AVPixelFormat pixFmt, uint32_t width, uint32_t height, std::vector<std::tuple<std::string, std::string, int32_t>> opts = {}, bool encoder = false);

    int32_t open();

    [[nodiscard]] AVCodecContext* getCodecContext() const;
    [[nodiscard]] AVCodec* getCodec() const;

    ~avcodec();
private:
    bool opened;

    AVCodecContext* codecContext;
    AVCodec* codec;
};
