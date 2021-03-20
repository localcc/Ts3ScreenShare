//
// Created by localcc on 3/20/21.
//

#pragma once
#include <cstdint>
#include <stdexcept>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

class avcodec {
public:
    avcodec(AVCodecParameters* codecParameters);

    int32_t open();

    AVCodecContext* getCodecContext() const;
    AVCodec* getCodec() const;

    ~avcodec();
private:
    bool opened;

    AVCodecContext* codecContext;
    AVCodec* codec;
};