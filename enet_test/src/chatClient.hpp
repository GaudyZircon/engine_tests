#ifndef _CHAT_CLIENT_HPP
#define _CHAT_CLIENT_HPP

#include "host.hpp"

class ChatClient : public Host {

    public:
        ChatClient();

    private:
        void handleConnect(ENetPeer *peer);
        void handleReceive(ENetPeer *peer, ENetPacket *packet, uint8_t channelID);
        void handleDisconnect(ENetPeer *peer);
};

#endif
