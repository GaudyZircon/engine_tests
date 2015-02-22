#include "chatServer.hpp"
#include <iostream>
#include <sstream>

ChatServer::ChatServer(uint16_t port) : Host(HOST_TYPE_SERVER, port)
{}

std::string getHost(const ENetAddress *address) {
    char str[1000];
    enet_address_get_host (address, str, 1000);
    return std::string(str);
}

void ChatServer::handleConnect(ENetPeer *peer) {
    std::stringstream ss;
    ss << getHost(&peer->address)
       << ":" << peer->address.port << " connected to the server!";
    ENetPacket *packet = enet_packet_create(ss.str().c_str(), ss.str().size()+1, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(host, 0, packet);
    std::cout << "[SERVER] " << ss.str() << std::endl;
}

void ChatServer::handleReceive(ENetPeer *peer, ENetPacket *packet, uint8_t channelID) {
    ENetPacket *p = enet_packet_create(packet->data, packet->dataLength, packet->flags);
    enet_host_broadcast(host, channelID, p);
    std::cout << "[SERVER] " << packet->data << std::endl;
}

void ChatServer::handleDisconnect(ENetPeer *peer) {
    std::stringstream ss;
    ss << getHost(&peer->address)
       << ":" << peer->address.port << " disconnected from the server!";
    ENetPacket *packet = enet_packet_create(ss.str().c_str(), ss.str().size()+1, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(host, 0, packet);
    std::cout << "[SERVER] " << ss.str() << std::endl;
}

