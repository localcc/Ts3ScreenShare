//
// Created by localcc on 3/20/21.
//

#include <capture/avcodec.h>

avcodec::avcodec(AVCodecParameters* codecParameters) {
    this->codec = avcodec_find_decoder(codecParameters->codec_id);
    if(!this->codec) {
        throw std::runtime_error("Failed to init decoder!");
    }

    this->codecContext = avcodec_alloc_context3(this->codec);
    if(!this->codecContext) {
        throw std::runtime_error("Failed to init codec context!");
    }

    this->codecContext->pix_fmt = static_cast<AVPixelFormat>(codecParameters->format);
    this->codecContext->width = codecParameters->width;
    this->codecContext->height = codecParameters->height;
}

int32_t avcodec::open() {
    int res =  avcodec_open2(this->codecContext, this->codec, nullptr);
    this->opened = res >= 0;
    return res;
}

avcodec::~avcodec() {
    if(this->opened) {
        avcodec_close(this->codecContext);
    }
    avcodec_free_context(&this->codecContext);
}