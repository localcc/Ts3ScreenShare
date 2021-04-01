#pragma once
#include <cstdint>
#include <stdexcept>
#include <enet.h>

class NetworkHelper {
public:

    enum EventTypes {
        EVENT_TYPE_LOGIN,
        EVENT_TYPE_FRAME,
        EVENT_TYPE_START_WATCHING,
        EVENT_TYPE_STOP_WATCHING,
        EVENT_TYPE_CREATE_WINDOW,
        EVENT_TYPE_START_STREAM,
        EVENT_TYPE_STOP_STREAM,
    };

    static ENetHost* CreateHostEx(uint16_t port, size_t clientLim, size_t channelNum, size_t down, size_t up) {
        ENetAddress address{};
        address.host = ENET_HOST_ANY;
        address.port = port;

        ENetHost* host = enet_host_create(&address, clientLim, channelNum, down, up);
        if(host == nullptr) {
            throw std::runtime_error("Failed to create host!");
        }
        return host;
    }

    static ENetHost* CreateClientHostEx(size_t channelNum, size_t down, size_t up) {
        ENetHost* host = enet_host_create(nullptr, 1, channelNum, down, up);
        if(host == nullptr) {
            throw std::runtime_error("Failed to create host!");
        }
        return host;
    }

};
