#pragma once
#include <SDL_net.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include "CameraPosition.h"
#include "Vector.h"
#include "PlayerInfo.h"

class NetworkManager {
public:
    bool initClient(const char* ip, int port,bool& isRunning, CameraPosition& client, PlayerInfo& gameInfo);
    void clearClient();
private:
    static void sendPosition(UDPsocket udpSocket, IPaddress serverIP, CameraPosition& client);
    static void sendMessage(UDPsocket udpSocket, IPaddress serverIP, std::string message);
    void clientSend(UDPsocket udpSocket, IPaddress* serverIP, bool& running, CameraPosition& client);
    bool parseUDPPacket(UDPsocket udpSocket, UDPpacket* packet, PlayerInfo& gameInfo);
    PlayerInfo parseData(std::string s);
    bool receive(UDPsocket udpSocket, UDPpacket* receivePacket, bool& isRunning, PlayerInfo& gameInfo);
    IPaddress serverIP;
    UDPsocket udpSocket;
    UDPpacket* receivePacket;
};