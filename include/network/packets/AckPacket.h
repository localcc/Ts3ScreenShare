#pragma once
#include <network/NetworkHelper.h>

struct __attribute__((packed)) AckPacket {
    uint8_t packetId = NetworkHelper::EVENT_TYPE_ACK;
};
