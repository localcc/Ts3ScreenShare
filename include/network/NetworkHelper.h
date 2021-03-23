#pragma once
#include <cstdint>
#include <stdexcept>

class NetworkHelper {
public:

    enum EventTypes {
        EVENT_TYPE_LOGIN,
        EVENT_TYPE_FRAME,
        EVENT_TYPE_START_WATCHING,
        EVENT_TYPE_STOP_WATCHING,
        EVENT_TYPE_CREATE_WINDOW,
        EVENT_TYPE_START_STREAM,
        EVENT_TYPE_ACK
    };
};
