#pragma once
#include <network/NetworkHelper.h>
#include <cstdint>

#ifdef __GNUC__
struct __attribute__((packed)) StartWatchingPacket {
#elif _MSC_VER
#pragma pack(push, 1)
struct StartWatchingPacket {
#endif
    uint8_t packetId = NetworkHelper::EVENT_TYPE_START_WATCHING;
    uint16_t clientId;
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif
