//
// Created by localcc on 3/20/21.
//
#pragma once
#include <libavformat/avformat.h>
#include <string>
#include <map>
#include <stdexcept>
#include <cstdint>

class avinput {
public:
    avinput(const std::string&& url, const std::string&& input_format, std::map<std::string, std::string> dictOptions);

    int32_t find_best_stream(enum AVMediaType type, int wanted_stream_nb, int related_stream, int flags);

    [[nodiscard]] const AVFormatContext* getFormatContext() const;

    ~avinput();
private:
    AVFormatContext* formatContext;
};
