#pragma once
#include <network/NetworkHelper.h>
#include <cstdint>

#ifdef __GNUC__
struct __attribute__((packed)) StartStreamPacket {
#elif _MSC_VER
#pragma pack(push, 1)
struct StartStreamPacket {
#endif
    uint8_t packetId = NetworkHelper::EVENT_TYPE_START_STREAM;
    int32_t width;
    int32_t height;
    int32_t pixFmt;
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif