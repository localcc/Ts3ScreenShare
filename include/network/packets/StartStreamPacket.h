#pragma once
#include <network/NetworkHelper.h>
#include <cstdint>

struct __attribute__((packed)) StartStreamPacket {
    uint8_t packetId = NetworkHelper::EVENT_TYPE_START_STREAM;
    int32_t width;
    int32_t height;
    int32_t pixFmt;
};
