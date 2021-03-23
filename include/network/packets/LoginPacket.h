#pragma once
#include <network/NetworkHelper.h>
#include <cstdint>

struct __attribute__((packed)) LoginPacket {
    uint8_t packetId = NetworkHelper::EVENT_TYPE_LOGIN;
    uint16_t clientId;
};
