#include "NetworkManager.h"

void NetworkManager::sendPosition(UDPsocket udpSocket, IPaddress serverIP, CameraPosition& client){
    std::string msg_data = "";// word.to_string();
    msg_data += std::to_string(1) + "%" +
                std::to_string(client.position.x) + "*" +
                std::to_string(client.position.y) + "*" +
                std::to_string(client.position.z) + "*" +
                std::to_string(client.theta) + "*" +
                std::to_string(client.gamma) + "@";

    // Create a UDP packet to send the position to the server
    UDPpacket* packet = SDLNet_AllocPacket(1024);
    if (packet) {
        packet->address = serverIP;
        packet->len = msg_data.size();
        packet->maxlen = msg_data.size();
        memcpy(packet->data, msg_data.c_str(), msg_data.size());

        if (SDLNet_UDP_Send(udpSocket, -1, packet) == 0) {
            std::cerr << "SDLNet_UDP_Send error: " << SDLNet_GetError() << std::endl;
        }

        SDLNet_FreePacket(packet);
    }
}

void NetworkManager::clearClient() {
    SDLNet_UDP_Close(udpSocket);
}

void NetworkManager::clientSend(UDPsocket udpSocket, IPaddress* serverIP, bool& running, CameraPosition& client) {
    while (running) {
        NetworkManager::sendPosition(udpSocket, *serverIP, client);
        SDL_Delay(10);
    }
}

bool NetworkManager::parseUDPPacket(UDPsocket udpSocket, UDPpacket* packet, PlayerInfo& gameInfo) {
    if (packet && packet->data) {
        char* txt = reinterpret_cast<char*>(packet->data);
        std::string packetData(txt);
        gameInfo = parseData(packetData);
    }
    return true;
}

PlayerInfo NetworkManager::parseData(std::string s) {
    char vecDelimiter = '*';
    std::string delimiter = "%";
    std::string finString = "@";
    std::string arrayOfSubStr[5];
    int index = 0;
    std::string s2 = s.substr(s.find(delimiter) + 1,s.find(finString) - 2);
    std::istringstream stream(s2);
    std::string token;
   while (getline(stream, token, vecDelimiter)) {
        arrayOfSubStr[index++] = token;
    }

    return PlayerInfo(std::stoi(s.substr(0, s.find(delimiter))),
                        MATH::Vec3{std::stof(arrayOfSubStr[0]) + 1,
                             std::stof(arrayOfSubStr[1]),
                             std::stof(arrayOfSubStr[2])},
                        std::stof(arrayOfSubStr[3]),
                        std::stof(arrayOfSubStr[4]));
}

bool NetworkManager::receive(UDPsocket udpSocket, UDPpacket* receivePacket, bool& isRunning, PlayerInfo& gameInfo) {
    while (isRunning) {
        int recieved = SDLNet_UDP_Recv(udpSocket, receivePacket);
        if (recieved) {
            SDL_Log("Recieve %i", recieved);
            NetworkManager::parseUDPPacket(udpSocket, receivePacket, gameInfo);
        }
    }

    SDLNet_FreePacket(receivePacket);
    
    return true;
}

bool NetworkManager::initClient(const char* ip, int port, bool& isRunning, CameraPosition& client, PlayerInfo& gameInfo) {

    if (SDLNet_Init() < 0) {
        SDL_Log("SDLNet initialization failed: %s", SDLNet_GetError());
        return false;
    }

    udpSocket = SDLNet_UDP_Open(0);
    receivePacket = SDLNet_AllocPacket(1024);

    if (!udpSocket) {
        SDL_Log("SDLNet_UDP_Open error: %s", SDLNet_GetError());
        SDLNet_Quit();
        return false;
    }

    if (SDLNet_ResolveHost(&serverIP, ip, port) < 0) {
        std::cerr << "SDLNet_ResolveHost error: " << SDLNet_GetError() << std::endl;
        SDLNet_UDP_Close(udpSocket);
        SDL_Quit();
        return false;
    }

    std::thread(&NetworkManager::clientSend, this, udpSocket, &serverIP, std::ref(isRunning), std::ref(client)).detach();
    std::thread(&NetworkManager::receive, this, udpSocket, receivePacket, std::ref(isRunning), std::ref(gameInfo)).detach();
    return true;
}