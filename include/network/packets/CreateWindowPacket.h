#pragma once
#include <network/NetworkHelper.h>
#include <cstdint>

#ifdef __GNUC__
struct __attribute__((packed)) CreateWindowPacket {
#elif _MSC_VER
#pragma pack(push, 1)
struct CreateWindowPacket {
#endif
    uint8_t packetId = NetworkHelper::EVENT_TYPE_CREATE_WINDOW;
    uint16_t clientId;
    uint32_t width;
    uint32_t height;
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif
