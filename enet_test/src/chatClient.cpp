#include "chatClient.hpp"
#include <iostream>
#include <sstream>

ChatClient::ChatClient() : Host(HOST_TYPE_CLIENT)
{}

void ChatClient::handleConnect(ENetPeer *peer) {
    std::cout << "[CLIENT] Connected to server!" << std::endl;
}

void ChatClient::handleReceive(ENetPeer *peer, ENetPacket *packet, uint8_t channelID) {
    std::cout << "[CLIENT] " << packet->data << std::endl;
}

void ChatClient::handleDisconnect(ENetPeer *peer) {
    std::cout << "[CLIENT] Disconnected from server!" << std::endl;
}

