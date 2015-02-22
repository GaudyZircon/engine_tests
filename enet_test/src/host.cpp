#include "host.hpp"
#include <iostream>

Host::Host(Host_type hostType, 
           uint16_t port, 
           size_t maxClients, 
           uint8_t maxChannels, 
           uint32_t pollTime) :
    hostType(hostType), pollTime(pollTime), bStopHost(false)
{
    if (hostType == HOST_TYPE_SERVER) {
        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = port;
        host = enet_host_create (& address,     /* the address to bind the host to */
                maxClients,                     /* allow up to maxClients and/or outgoing connections */
                maxChannels,                    /* allow up to maxChannels channels to be used, starting at 0 */
                0,                              /* assume any amount of incoming bandwidth */
                0);                             /* assume any amount of outgoing bandwidth */
    } else if (hostType == HOST_TYPE_CLIENT) {
        host = enet_host_create(NULL, 1, maxChannels, 0, 0);
    } else {
        std::cerr << "An error occurred while trying to create an ENet host : Unknown host type." << std::endl;
    }

    if (host == NULL) {
        std::cerr << "An unknown error occurred while trying to create an ENet host." << std::endl;
        exit(EXIT_FAILURE);
    }
}
        
Host::Host(Host_type hostType, uint8_t maxChannels, uint32_t pollTime) :
    Host(hostType, 8888, 1, maxChannels, pollTime)
{
    if (hostType != HOST_TYPE_CLIENT) {
        std::cerr << "Warning : Creating a server host with default port (8888) and only one client." << std::endl;
    }
}

Host::~Host() {
    stop();
    enet_host_destroy(host);
}

void Host::start() {
    hostThread = std::thread(&Host::loop, this);
}

void Host::stop() {
    bStopHost = true;
    if (hostThread.joinable()) hostThread.join();
}

void Host::loop() {
    ENetEvent event;
    while (true) {
        auto t = enet_time_get();
        // Handle events
        while (poll(event)) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    handleConnect(event.peer);
                    connectedPeers.insert(event.peer);
                    pendingConnections.erase(event.peer);
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    handleReceive(event.peer, event.packet, event.channelID);
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    handleDisconnect(event.peer);
                    connectedPeers.erase(event.peer);
                    break;
                default:
                    break;
            }
        }
        // Handle pending connections
        auto eventTime = enet_time_get() - t;
        for (auto itPeer : pendingConnections) {
            itPeer.second -= eventTime;
            if (itPeer.second < 0) {
                std::cerr << "Failed to connect to " << itPeer.first->address.host 
                          << ":" << itPeer.first->address.port << std::endl;
                failedConnections[itPeer.first]++;
                pendingConnections.erase(itPeer.first);
            }
        }
        // Check if we need to stop the host
        if (bStopHost) {
            break;
        }
#ifdef DEBUG
        // Stats
        static auto timeRef = 0;
        static int ticks = 0;
        ticks++;
        if (enet_time_get() - timeRef > 2000) {
            if (hostType == HOST_TYPE_SERVER) {
                auto tickRate = 1000*ticks/(enet_time_get()-timeRef);
                std::cout << "TickRate (tick/s) = " << tickRate << std::endl;
                timeRef = enet_time_get();
                ticks = 0;
            }
            std::cout << "Connections : " << connectedPeers.size() << std::endl;
        }
#endif
    }
    // Send all packets before disconnecting
    for (ENetPeer *peer : connectedPeers) {
        enet_peer_disconnect_later(peer, 0);
    }
    // Wait until we're not connected to anyone anymore
    // Drop incoming packets
    while (connectedPeers.size() > 0) {
        while (poll(event)) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE:
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    connectedPeers.erase(event.peer);
                    break;
                default:
                    break;
            }
        }
    }
}

bool Host::poll(ENetEvent &event) {
    return (enet_host_service(host, &event, pollTime) > 0);
}
        
bool Host::connectToHost(std::string hostAddress, uint16_t port, uint8_t channelCount, uint32_t timeout) {
    ENetAddress address;
    ENetPeer *peer;
    enet_address_set_host(&address, hostAddress.c_str());
    address.port = port;
    peer = enet_host_connect(host, &address, channelCount, 0);
    if (peer == NULL) {
        std::cerr << "Error : No available peers for initiating an ENet connection." << std::endl;
        return false;
    }
    pendingConnections[peer] = timeout;
    // Wait until connection is either established or failed
    while (pendingConnections.find(peer) != pendingConnections.end());
    if (failedConnections.find(peer) == failedConnections.end())
        return true;
    return false;
}

Packet Host::createPacket(const void *data, size_t size, ENetPacketFlag flag) {
    Packet p = enet_packet_create((const uint8_t*)data, size, flag);
    if (p == NULL)
        std::cerr << "Error : Can not create packet." << std::endl;
    return p;
}

int Host::sendPacket(Peer peer, Packet packet, uint8_t channelID) {
    if (connectedPeers.find(peer) == connectedPeers.end()) {
        std::cerr << "Error : Invalid peer for sending packet." << std::endl;
        return -1;
    }
    return enet_peer_send(peer, channelID, packet);
}

void Host::broadcastPacket(Packet packet, uint8_t channelID) {
    enet_host_broadcast(host, channelID, packet);
}

