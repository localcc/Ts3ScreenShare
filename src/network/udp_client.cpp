//
// Created by localcc on 3/20/21.
//

#include <network/udp_client.h>
#include <stdexcept>
#include <network/NetworkHelper.h>
#include <network/packets/LoginPacket.h>
#include <network/packets/CreateWindowPacket.h>
#include <network/packets/StartWatchingPacket.h>
#include <network/packets/StopWatchingPacket.h>
#include <iostream>


udp_client::udp_client() : host(NetworkHelper::CreateClientHostEx(2, 0, 0)), running(false)
{
}

bool udp_client::sock_connect(std::string &&hostname, uint16_t port, uint16_t clientId) {
    if(this->running) {
        std::cout << "Already connected!" << std::endl;
        return true;
    }

    ENetAddress address{};
    address.port = port;
    enet_address_set_host(&address, hostname.c_str());

    this->peer = std::unique_ptr<ENetPeer>(enet_host_connect(this->host, &address, 2, 0));
    if(this->peer == nullptr) {
        return false;
    }


    this->running = true;
    uint16_t cid = clientId;
    this->connectedThread = std::thread([&](){
        uint16_t c = cid;
        ENetEvent event;
        int32_t res = enet_host_service(host, &event, 0);

        while(this->running && res >= 0) {
            switch(event.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    LoginPacket packet{.clientId = c};
                    this->sock_write(reinterpret_cast<uint8_t*>(&packet), sizeof(LoginPacket));
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    switch(static_cast<NetworkHelper::EventTypes>(event.packet->data[0])) {
                        case NetworkHelper::EVENT_TYPE_CREATE_WINDOW: {
                            std::cout << "Create window!" << std::endl;
                            auto* packet = reinterpret_cast<CreateWindowPacket*>(event.packet->data);
                            this->handle_create_window(packet->clientId, packet->width, packet->height);
                            break;
                        }
                        case NetworkHelper::EVENT_TYPE_FRAME: {
                            auto* packet = reinterpret_cast<FlatFramePacket*>(event.packet->data);
                            this->handle_frame(packet, event.packet->data + sizeof(FlatFramePacket), event.packet->dataLength - sizeof(FlatFramePacket));
                            break;
                        }
                        default:
                            break;
                    }

                    enet_packet_destroy(event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT:
                    this->running = false;
                    break;
                case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                    this->running = false;
                    break;
                case ENET_EVENT_TYPE_NONE:
                    break;
            }
            res = enet_host_service(host, &event, 3);
        }
        std::cout << "Stopped!" << std::endl;
    });

    return true;
}

int32_t udp_client::sock_write(const uint8_t *data, int64_t size) {
    ENetPacket* packet = enet_packet_create(data, size, 0);
    return enet_peer_send(this->peer.get(), 0, packet);
}

udp_client::~udp_client() {
    this->running = false;
    this->connectedThread.join();
    enet_peer_disconnect(this->peer.get(), 0);
}

void udp_client::start_watching(uint16_t clientId) {
    if(std::find(this->waitingForCreation.begin(), this->waitingForCreation.end(), clientId) != this->waitingForCreation.end()) return;
    this->waitingForCreation.push_back(clientId);

    StartWatchingPacket packet{.clientId = clientId};
    std::cout << "Locking for writing!" << std::endl;
    this->sock_write(reinterpret_cast<uint8_t*>(&packet), sizeof(StartWatchingPacket));
    std::cout << "Unlocked!" << std::endl;
}

void udp_client::handle_frame(FlatFramePacket* packet, uint8_t* data, uint64_t dataLength)
{
    std::cout << "Handling frame prologue" << std::endl;
    const auto videoWindow = this->videoWindows.find(packet->clientId);
    if(videoWindow == this->videoWindows.end()) {
        std::cout << "No window!" << std::endl;
        return;
    }

    videoWindow->second->handle_frame(packet->pts, packet->dts, packet->flags, data, dataLength);
    if(videoWindow->second->getClosing()) {
        this->stop_watching(packet->clientId);
    }
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

    auto window = std::make_unique<video_window>("Video stream of: " + std::to_string(clientId), width, height);
    this->videoWindows.emplace(clientId, std::move(window));
}

void udp_client::stop_watching(uint16_t clientId) {
    StopWatchingPacket packet{.clientId = clientId};
    this->videoWindows.erase(clientId);

    this->sock_write(reinterpret_cast<uint8_t*>(&packet), sizeof(StopWatchingPacket));
}
