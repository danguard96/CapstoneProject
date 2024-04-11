//#include "ServerNetworkManager.h"
//
//bool NetworkManager::parseUDPPacket(UDPsocket udpSocket, UDPpacket* packet, PlayerInfo& gameInfo) {
//    if (packet && packet->data) {
//        char* txt = reinterpret_cast<char*>(packet->data);
//        std::string packetData(txt);
//
//        gameInfo.ump.insert_or_assign(packet->address.port, parseData(packetData));
//
//    return false;
//}
//
//PlayerInfo NetworkManager::parseData(std::string s){
//    char vecDelimiter = '*';
//    std::string delimiter = "%";
//    std::string finString = "@";
//    std::string arrayOfSubStr[5];
//    int index = 0;
//    std::string s2 = s.substr(s.find(delimiter) + 1,s.find(finString) - 2);
//    std::istringstream stream(s2);
//    std::string token;
//   while (getline(stream, token, vecDelimiter)) {
//        // Add the token to the array 
//        arrayOfSubStr[index++] = token;
//    }
//
//    return PlayerInfo(std::stoi(s.substr(0, s.find(delimiter))),
//                        MATH::Vec3{std::stof(arrayOfSubStr[0]) + 1,
//                             std::stof(arrayOfSubStr[1]),
//                             std::stof(arrayOfSubStr[2])},
//                        std::stof(arrayOfSubStr[3]),
//                        std::stof(arrayOfSubStr[4]));
//}
//
//bool NetworkManager::receive(UDPsocket udpSocket, UDPpacket* receivePacket, PlayerInfo& gameInfo) {
//    int recieved = SDLNet_UDP_Recv(udpSocket, receivePacket);
//    if (recieved) {
//        SDL_Log("Recieve %i", recieved);
//        NetworkManager::parseUDPPacket(udpSocket, receivePacket, gameInfo);
//        return true;
//    }
//    return false;
//}
//
//void NetworkManager::handleNetwork(UDPsocket udpSocket, bool& running, PlayerInfo& gameInfo) {
//    while (running) {
//        NetworkManager::receive(udpSocket, receivePacket, gameInfo);
//    }
//
//    SDLNet_FreePacket(receivePacket);
//}
//
//bool NetworkManager::initServer(bool& isRunning) {
//
//    if (SDLNet_Init() < 0) {
//        SDL_Log("SDLNet initialization failed: %s", SDLNet_GetError());
//        return false;
//    }
//
//    udpSocket = SDLNet_UDP_Open(8080);
//    receivePacket = SDLNet_AllocPacket(1024);
//
//    if (!udpSocket) {
//        SDL_Log("SDLNet_UDP_Open error: %s", SDLNet_GetError());
//        SDLNet_Quit();
//        return false;
//    }
//
//    if (!receivePacket) {
//        SDL_Log("SDLNet_AllocPacket error: %s", SDLNet_GetError());
//        SDLNet_UDP_Close(udpSocket);
//        SDL_Quit();
//        return false;
//    }
//
//    networkThread = std::thread(&NetworkManager::handleNetwork, this, udpSocket, std::ref(isRunning), std::ref(gi));
//    networkThread.detach();
//    return true;
//}
//
//void NetworkManager::clearServer() {
//    SDLNet_UDP_Close(udpSocket);
//}
