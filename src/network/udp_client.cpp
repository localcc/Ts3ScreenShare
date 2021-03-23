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
#include <iostream>


udp_client::udp_client() : host(NetworkHelper::CreateClientHostEx(2, 0, 0), &enet_host_destroy), running(false)
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

    this->peer = std::unique_ptr<ENetPeer>(enet_host_connect(this->host.get(), &address, 2, 0));
    if(this->peer == nullptr) {
        return false;
    }

    this->running = true;
    uint16_t cid = clientId;
    this->connectedThread = std::thread([&](){
        auto* host = this->host.get();
        ENetEvent event;

        while(this->running && enet_host_service(host, &event, 0) >= 0) {
            switch(event.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    LoginPacket packet{.clientId = cid};
                    this->sock_write(reinterpret_cast<uint8_t*>(&packet), sizeof(LoginPacket));
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    switch(static_cast<NetworkHelper::EventTypes>(event.packet->data[0])) {
                        case NetworkHelper::EVENT_TYPE_CREATE_WINDOW: {
                            CreateWindowPacket* packet = reinterpret_cast<CreateWindowPacket*>(event.packet->data);
                            this->handle_create_window(packet->clientId, packet->width, packet->height);
                            break;
                        }
                        case NetworkHelper::EVENT_TYPE_FRAME: {
                            FlatFramePacket* packet = reinterpret_cast<FlatFramePacket*>(event.packet->data);
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
        }
    });

    return true;
}

int32_t udp_client::sock_write(const uint8_t *data, int64_t size) const {
    ENetPacket* packet = enet_packet_create(data, size, 0);
    return enet_peer_send(this->peer.get(), 0, packet);
}

udp_client::~udp_client() {
    this->running = false;
    this->connectedThread.join();
    enet_peer_disconnect(this->peer.get(), 0);
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
