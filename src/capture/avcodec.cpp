//
// Created by localcc on 3/20/21.
//

#include <capture/avcodec.h>

avcodec::avcodec(AVCodecParameters* codecParameters, bool encoder) {
    if(encoder) {
        this->codec = avcodec_find_encoder(codecParameters->codec_id);
    }else {
        this->codec = avcodec_find_decoder(codecParameters->codec_id);
    }
    if(!this->codec) {
        throw std::runtime_error("Failed to init codec!");
    }

    this->codecContext = avcodec_alloc_context3(this->codec);
    if(!this->codecContext) {
        throw std::runtime_error("Failed to init codec context!");
    }

    this->codecContext->pix_fmt = static_cast<AVPixelFormat>(codecParameters->format);
    this->codecContext->width = codecParameters->width;
    this->codecContext->height = codecParameters->height;
}

avcodec::avcodec(AVCodecID id, AVPixelFormat pixFmt, uint32_t width, uint32_t height, bool encoder)
{
    if(encoder) {
        this->codec = avcodec_find_encoder(id);
    }else {
        this->codec = avcodec_find_decoder(id);
    }
    if(!this->codec) {
        throw std::runtime_error("Failed to init codec!");
    }

    this->codecContext = avcodec_alloc_context3(this->codec);
    if(!this->codecContext) {
        throw std::runtime_error("Failed to init codec context!");
    }

    this->codecContext->pix_fmt = pixFmt;
    this->codecContext->width = width;
    this->codecContext->height = height;
}

int32_t avcodec::open() {
    int res =  avcodec_open2(this->codecContext, this->codec, nullptr);
    this->opened = res >= 0;
    return res;
}

AVCodec * avcodec::getCodec() const {
    return this->codec;
}

AVCodecContext * avcodec::getCodecContext() const {
    return this->codecContext;
}

avcodec::~avcodec() {
    if(this->opened) {
        avcodec_close(this->codecContext);
    }
    avcodec_free_context(&this->codecContext);
}
