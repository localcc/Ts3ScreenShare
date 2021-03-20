//
// Created by localcc on 3/20/21.
//
#include <capture/avinput.h>

avinput::avinput(const std::string&& url, const std::string&& input_format, const std::map<std::string, std::string> dictOptions) {
    auto* const inputFormat = av_find_input_format(input_format.c_str());
    this->formatContext = avformat_alloc_context();

    AVDictionary* dict = nullptr;
    for(auto& pair : dictOptions) {
        av_dict_set(&dict, pair.first.c_str(), pair.second.c_str(), 0);
    }
    int res = avformat_open_input(&formatContext, url.c_str(), inputFormat, &dict);
    if(res < 0) {
        throw std::runtime_error("Failed to open input!");
    }
    av_dict_free(&dict);
}

int32_t avinput::find_best_stream(enum AVMediaType type, int wanted_stream_nb, int related_stream, int flags) {
    return av_find_best_stream(this->formatContext, type, wanted_stream_nb, related_stream, nullptr, flags);
}

const AVFormatContext * avinput::getFormatContext() const {
    return this->formatContext;
}

avinput::~avinput() {
    avformat_free_context(this->formatContext);
}