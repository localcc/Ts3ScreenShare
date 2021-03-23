#include <capture/swscontext.h>


swscontext::swscontext(uint32_t srcW, uint32_t srcH, AVPixelFormat srcFormat, uint32_t dstW, uint32_t dstH,
                       AVPixelFormat dstFormat, int32_t flags) : srcW(srcW), srcH(srcH), srcFormat(srcFormat),
                        dstW(dstW), dstH(dstH), dstFormat(dstFormat), flags(flags) {
    this->context = sws_getCachedContext(nullptr, srcW, srcH, srcFormat, dstW, dstH, dstFormat, flags, nullptr, nullptr, nullptr);
    if(this->context == nullptr) {
        throw std::runtime_error("Failed to get context");
    }
}

SwsContext* swscontext::get() {
    this->context = sws_getCachedContext(this->context, srcW, srcH, srcFormat, dstW, dstH, dstFormat, flags, nullptr, nullptr, nullptr);
    return this->context;
}

swscontext::~swscontext() {
    sws_freeContext(this->context);
}
