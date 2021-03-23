#pragma once
#include <network/NetworkHelper.h>
#include <cstdint>

struct __attribute__((packed)) StopWatchingPacket {
    uint8_t packetId = NetworkHelper::EVENT_TYPE_STOP_WATCHING;
    uint16_t clientId;
};
