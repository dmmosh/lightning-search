#include "MultiHeader.h"
#include <iostream>


void MultiClientChat::onClientConnected(int client){ // client connected
    sendToClient(client, "welcome",9);
    std::cout << "client connected to server\n";
};

void MultiClientChat::onClientDisconnected(int client){ // client disconnected


};


void MultiClientChat::onMessageReceived(int client, const char* msg, int length){ // message is received from client
broadcastToClients(client,msg,length);

};
