#pragma once
#include <cstdint>
#include <network/NetworkHelper.h>

struct __attribute__((packed)) FlatFramePacket {
    uint8_t packetId = NetworkHelper::EVENT_TYPE_FRAME;
    uint16_t clientId;
    int64_t pts;
    int64_t dts;
    int32_t flags;
};
