#pragma once
#include <network/NetworkHelper.h>
#include <cstdint>

struct __attribute__((packed)) CreateWindowPacket {
    uint8_t packetId = NetworkHelper::EVENT_TYPE_CREATE_WINDOW;
    uint16_t clientId;
    uint32_t width;
    uint32_t height;
};
