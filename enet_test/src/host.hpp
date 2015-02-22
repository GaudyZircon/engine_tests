#ifndef _ENET_SERVER_HPP
#define _ENET_SERVER_HPP

#include <string>
#include <set>
#include <unordered_map>
#include <thread>
#include <enet/enet.h>

enum Host_type {
    HOST_TYPE_SERVER = 0,   /* Can connect to and be connected to another host */
    HOST_TYPE_CLIENT = 1    /* Can not be connected to by another host and has only one connection*/
};

typedef ENetPacket* Packet;
typedef ENetPeer* Peer;

class Host {

    public:
        Host(Host_type hostType, 
             uint16_t port, 
             size_t maxClients = 128, 
             uint8_t maxChannels = 2,
             uint32_t pollTime = 0 
             );
        Host(Host_type hostType, 
             uint8_t maxChannels = 2,
             uint32_t pollTime = 0 
             );

        ~Host();

        void start();
        void stop();
       
        bool connectToHost(std::string address, uint16_t port, uint8_t channelCount = 2, uint32_t timeout = 1000);  
        Packet createPacket(const void *data, size_t size, ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE);
        int sendPacket(Peer peer, Packet packet, uint8_t channelID = 0);
        void broadcastPacket(Packet packet, uint8_t channelID = 0);

        const std::set<Peer> &getPeers() {return connectedPeers;}
       
    protected:
        virtual void handleConnect(ENetPeer *peer) = 0;
        virtual void handleReceive(ENetPeer *peer, ENetPacket *packet, uint8_t channelID) = 0;
        virtual void handleDisconnect(ENetPeer *peer) = 0;
        
        Host_type hostType;
        ENetHost *host;
        std::unordered_map<ENetPeer *, int32_t> pendingConnections;
        std::set<ENetPeer *> connectedPeers;
        std::unordered_map<ENetPeer *, uint32_t> failedConnections;
        uint32_t pollTime; // in ms

    private:
        void loop();
        bool poll(ENetEvent &event);

        bool bStopHost;
        std::thread hostThread;
};

#endif
