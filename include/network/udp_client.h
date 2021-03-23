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
#include <network/NetworkHelper.h>
#include <memory>
#include <thread>
#include <unordered_map>
#include <functional>
#include <network/packets/FlatFramePacket.h>
#include <window/video_window.h>

class udp_client {
public:
    udp_client();

    bool sock_connect(std::string&& hostname, int16_t port, uint16_t clientID);
    bool isConnected();
    void disconnect();

    int32_t sock_write(const uint8_t* data, int64_t size) const;
    int32_t sock_read(uint8_t* data, int64_t size, int32_t flags) const;

    void start_watching(uint16_t clientId);

    ~udp_client();
private:

    int32_t sock_fd;

    std::vector<uint16_t> waitingForCreation;
    std::unordered_map<uint16_t, std::unique_ptr<video_window>> videoWindows;

    void handle_frame(FlatFramePacket* packet, uint8_t* data, uint64_t dataLength);
    void handle_create_window(uint16_t clientId, uint32_t width, uint32_t height);

    bool running;
    std::thread connectedThread;
};
