#pragma once
#include <SDL2/SDL_net.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <bitset>
#include <sstream>
#include <iostream>
#include "Vector.h"
#include "PlayerInfo.h"

class NetworkManager {
public:
    bool initServer(bool& isRunning, PlayerInfo& playerInfo);
    void clearServer();
private:
    static bool parseUDPPacket(UDPsocket udpSocket, UDPpacket* packet, PlayerInfo& gameInfo);
    static bool receive(UDPsocket udpSocket, UDPpacket* receivePacket, PlayerInfo& gameInfo);
    void handleNetwork(UDPsocket udpSocket, bool& running, PlayerInfo& gameInfo);
    static PlayerInfo parseData(std::string s);
    UDPpacket* receivePacket;
    UDPsocket udpSocket;
    std::thread networkThread;
};