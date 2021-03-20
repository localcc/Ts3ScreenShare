//
// Created by localcc on 3/20/21.
//

#pragma once
#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
#include <Windows.h>
#elif defined(__linux) || defined(__linux__) || defined(__gnu_linux__) || defined(linux)
#include <sys/types.h>
#include <sys/socket.h>
#elif defined(__APPLE__)
//todo: include for apple
#endif
#include <cstdint>

class udp_client {
public:
    udp_client(int16_t port);

    int32_t sock_write(const uint8_t* data, int64_t size) const;
    int32_t sock_read(uint8_t* data, int64_t size) const;

    ~udp_client();
private:
    int32_t sock_fd;
};
