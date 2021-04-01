#pragma once
#include <cstdint>
#include <network/NetworkHelper.h>

#ifdef __GNUC__
struct __attribute__((packed)) FlatFramePacket {
#elif _MSC_VER
#pragma pack(push, 1)
struct FlatFramePacket {
#endif
    uint8_t packetId = NetworkHelper::EVENT_TYPE_FRAME;
    uint16_t clientId;
    int64_t pts;
    int64_t dts;
    int32_t flags;
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif