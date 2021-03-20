//
// Created by localcc on 3/20/21.
//

#include <network/udp_client.h>
#include <stdexcept>
#include <unistd.h>

udp_client::udp_client(int16_t port) {
    //todo: add ipv6 support
    this->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(this->sock_fd < 0) {
        throw std::runtime_error("Failed to initialize socket!");
    }
}

int32_t udp_client::sock_write(const uint8_t *data, int64_t size) const {
    return write(this->sock_fd, data, size);
}

int32_t udp_client::sock_read(uint8_t *data, int64_t size) const {
    return read(this->sock_fd, data, size);
}

udp_client::~udp_client() {
    shutdown(this->sock_fd, SHUT_RDWR);
    close(this->sock_fd);
}