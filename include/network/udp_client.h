//
// Created by localcc on 3/20/21.
//

#pragma once
#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
#include <Windows.h>
#elif defined(__linux) || defined(__linux__) || defined(__gnu_linux__) || defined(linux)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#elif defined(__APPLE__)
//todo: include for apple
#endif
#include <cstdint>
#include <string>

class udp_client {
public:
    udp_client(std::string&& host, int16_t port);

    int32_t sock_write(const uint8_t* data, int64_t size) const;
    int32_t sock_read(uint8_t* data, int64_t size) const;

    ~udp_client();
private:
    sockaddr_in addr;

    int32_t sock_fd;
};
