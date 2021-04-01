#pragma once

extern "C" {
#include <libswscale/swscale.h>
}
#include <stdexcept>

class swscontext {
public:

    swscontext(uint32_t srcW, uint32_t srcH, AVPixelFormat srcFormat, uint32_t dstW, uint32_t dstH, AVPixelFormat dstFormat, int32_t flags);

    SwsContext* get();
    void update(uint32_t srcW, uint32_t srcH, AVPixelFormat srcFormat, uint32_t dstW, uint32_t dstH, AVPixelFormat dstFormat, int32_t flags);

    ~swscontext();

private:
    uint32_t srcW;
    uint32_t srcH;
    AVPixelFormat srcFormat;

    uint32_t dstW;
    uint32_t dstH;
    AVPixelFormat dstFormat;

    int32_t flags;

    SwsContext* context;
};
