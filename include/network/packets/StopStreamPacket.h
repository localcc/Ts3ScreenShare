#pragma once
#include <cstdint>
#include <network/NetworkHelper.h>

struct __attribute__((packed)) StopStreamPacket {
    uint8_t packetId = NetworkHelper::EVENT_TYPE_STOP_STREAM;
};