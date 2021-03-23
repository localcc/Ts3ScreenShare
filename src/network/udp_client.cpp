//
// Created by localcc on 3/20/21.
//

#include <network/udp_client.h>
#include <stdexcept>
#include <unistd.h>
#include <network/NetworkHelper.h>
#include <network/packets/LoginPacket.h>
#include <network/packets/CreateWindowPacket.h>
#include <network/packets/StartWatchingPacket.h>
#include <network/packets/StopWatchingPacket.h>
#include <network/packets/StartStreamPacket.h>
#include <network/packets/AckPacket.h>
#include <iostream>


udp_client::udp_client()
{
    this->sock_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
    if(this->sock_fd < 0) {
        throw std::runtime_error("Failed to create socket!");
    }



    this->running = false;
}

bool udp_client::sock_connect(std::string &&hostname, int16_t port, uint16_t clientId) {
    if(this->running) {
        std::cout << "Already connected!" << std::endl;
        return true;
    }

    sockaddr_in addr{};
    addr.sin_port = port;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, hostname.c_str(), &addr.sin_addr.s_addr);

    const auto loginpacket = LoginPacket{.clientId = clientId};
    size_t size = send(this->sock_fd, &loginpacket, sizeof(LoginPacket), 0);
    if(size < 0) {
        return false;
    }

    AckPacket recvPacket{};

    size = recv(this->sock_fd, &recvPacket, sizeof(AckPacket), MSG_TRUNC);
    if(size < 0) {
        return false;
    }

    this->running = true;

    this->connectedThread = std::thread([&](){
        size_t recvSize;
        while(this->running) {
            recvSize = recv(this->sock_fd, nullptr, 0, MSG_PEEK | MSG_TRUNC);
            if(recvSize < 0) {
                if(errno == EAGAIN) continue;
            }

            auto* buf = new uint8_t[recvSize];
            recv(this->sock_fd, buf, recvSize, 0);

            switch(static_cast<NetworkHelper::EventTypes>(buf[0])) {
                case NetworkHelper::EVENT_TYPE_CREATE_WINDOW: {
                    CreateWindowPacket* packet = reinterpret_cast<CreateWindowPacket*>(buf);
                    this->handle_create_window(packet->clientId, packet->width, packet->height);
                    break;
                }
                case NetworkHelper::EVENT_TYPE_FRAME: {
                    FlatFramePacket* packet = reinterpret_cast<FlatFramePacket*>(buf);
                    this->handle_frame(packet, buf + sizeof(FlatFramePacket), recvSize - sizeof(FlatFramePacket));
                    break;
                }
                default:
                    break;
            }

            delete[] buf;
        }
    });

    return true;
}

int32_t udp_client::sock_write(const uint8_t *data, int64_t size) const {
    return send(this->sock_fd, data, size, 0);
}

int32_t udp_client::sock_read(uint8_t *data, int64_t size, int32_t flags) const {
    return recv(this->sock_fd, data, size, flags);
}


udp_client::~udp_client() {
    this->running = false;
    this->connectedThread.join();
    shutdown(this->sock_fd, SHUT_RDWR);
    close(this->sock_fd);
}

void udp_client::start_watching(uint16_t clientId) {
    this->waitingForCreation.push_back(clientId);

    StartWatchingPacket packet{.clientId = clientId};
    this->sock_write(reinterpret_cast<uint8_t*>(&packet), sizeof(StartWatchingPacket));
}

void udp_client::handle_frame(FlatFramePacket* packet, uint8_t* data, uint64_t dataLength)
{
    const auto videoWindow = this->videoWindows.find(packet->clientId);
    if(videoWindow == this->videoWindows.end()) return;

    //todo: handle frame
    std::cout << "Handling frame" << std::endl;
    videoWindow->second->handle_frame(packet->pts, packet->dts, packet->flags, data, dataLength);
}

bool udp_client::isConnected() {
    return this->running;
}

void udp_client::disconnect()
{
    if(this->running) {
        this->running = false;
        this->connectedThread.join();
    }
}

void udp_client::handle_create_window(uint16_t clientId, uint32_t width, uint32_t height)
{
    const auto removeRes = std::remove(this->waitingForCreation.begin(), this->waitingForCreation.end(), clientId);
    if(removeRes == this->waitingForCreation.end()) return;
    std::cout << "Width: " << width << " height: " << height << std::endl;
    this->videoWindows.emplace(clientId , std::make_unique<video_window>("suqa", width, height));
}
