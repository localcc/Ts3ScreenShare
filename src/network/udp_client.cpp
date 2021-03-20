//
// Created by localcc on 3/20/21.
//

#include <network/udp_client.h>
#include <stdexcept>
#include <unistd.h>

udp_client::udp_client(std::string&& host, int16_t port) {
    //todo: add ipv6 support
    this->sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(this->sock_fd < 0) {
        throw std::runtime_error("Failed to initialize socket!");
    }

    this->addr = {};
    this->addr.sin_family = AF_INET;
    this->addr.sin_port = port;
    inet_pton(AF_INET, host.c_str(), &this->addr.sin_addr);
}

int32_t udp_client::sock_write(const uint8_t *data, int64_t size) const {
    return sendto(this->sock_fd, data, size, MSG_DONTWAIT, reinterpret_cast<const sockaddr*>(&this->addr), sizeof(this->addr));
}

int32_t udp_client::sock_read(uint8_t *data, int64_t size) const {
    return recv(this->sock_fd, data, size, 0);
}

udp_client::~udp_client() {
    shutdown(this->sock_fd, SHUT_RDWR);
    close(this->sock_fd);
}
