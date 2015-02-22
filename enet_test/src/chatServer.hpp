#ifndef _CHAT_SERVER_HPP
#define _CHAT_SERVER_HPP

#include "host.hpp"

class ChatServer : public Host {

    public:
        ChatServer(uint16_t port);

    private:
        void handleConnect(ENetPeer *peer);
        void handleReceive(ENetPeer *peer, ENetPacket *packet, uint8_t channelID);
        void handleDisconnect(ENetPeer *peer);
};

#endif
