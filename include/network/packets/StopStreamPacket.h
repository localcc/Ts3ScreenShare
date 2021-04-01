#pragma once
#include <cstdint>
#include <network/NetworkHelper.h>

#ifdef __GNUC__
struct __attribute__((packed)) StopStreamPacket {
#elif _MSC_VER
#pragma pack(push, 1)
struct StopStreamPacket {
#endif
    uint8_t packetId = NetworkHelper::EVENT_TYPE_STOP_STREAM;
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif