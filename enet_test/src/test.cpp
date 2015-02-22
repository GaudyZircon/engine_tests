#include <iostream>
#include <unordered_map>
#include <string>
#include <cstring>
#include <thread>
#include <sstream>
#include <enet/enet.h>

#include "chatServer.hpp"
#include "chatClient.hpp"

/*
std::string rewrite(std::string s, std::unordered_map<char, std::string> m) {
    std::string res = "";
    for (char c : s) {
        auto it = m.find(c);
        if (it != m.end())
            res += it->second;
        else
            res += c;
    }
    return res;
}
*/


int main(void)
{
    if (enet_initialize() != 0) {
        std::cerr << "An error occurred while initializing ENet." << std::endl;
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ChatServer server(8888);
    server.start();
    ChatClient client, client2;
    client.start();
    client2.start();
    std::cout << "Connection status (1) : " << client.connectToHost("localhost", 8888) << std::endl;
    std::cout << "Connection status (2) : " << client2.connectToHost("localhost", 8888) << std::endl;
   
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); 

    int i = 0;
    while (i<=100000*10) {
        //do stuff
        /// Simulate periodic messages
        if (i++%100000 == 0) {
            std::stringstream ss;
            ss << "MESSAGE n°" << i/100000;
            uint8_t data[ss.str().size()+1];
            std::memcpy(data, ss.str().c_str(), ss.str().size()+1);
            Packet p = client.createPacket(data, ss.str().size()+1, ENET_PACKET_FLAG_RELIABLE);
            Packet p2 = client2.createPacket(data, ss.str().size()+1, ENET_PACKET_FLAG_RELIABLE);
            client.broadcastPacket(p, 0);
            client2.broadcastPacket(p2, 0);
        }
    }
    client.stop();
    client2.stop();

    /*
   std::unordered_map<char, std::string> m = {{'L', "L+R+"}, {'R',"-L+R"}};
   std::string s = "L";
   std::cout << "0 : " << s << std::endl;
   for (int i = 1; i <= 4; i++) {
        s = rewrite(s,m);
        std::cout << i << " : " << s << std::endl;
   }
   */
   return EXIT_SUCCESS;
}
