#pragma once
#include <network/NetworkHelper.h>
#include <cstdint>

struct __attribute__((packed)) StartWatchingPacket {
    uint8_t packetId = NetworkHelper::EVENT_TYPE_START_WATCHING;
    uint16_t clientId;
};
