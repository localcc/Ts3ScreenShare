//
// Created by localcc on 3/20/21.
//
#include <capture/screen_capture.h>

#include <libavformat/avformat.h>
#include <string>

screen_capture::screen_capture(int32_t width, int32_t height) {
    AVInputFormat* inputFormat = av_find_input_format("x11grab");
    AVFormatContext* formatContext = avformat_alloc_context();

    AVDictionary* dict = nullptr;
    std::string video_size("1680x1050");
    av_dict_set(&dict, "video_size", video_size.c_str(), 0);
}