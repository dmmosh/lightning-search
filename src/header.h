#pragma once
#include <cstdint>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cerrno> // For errno
#include <sstream>


class TcpListener{
    public:
        TcpListener(const char* ip, int port):
            m_ip(ip), m_port(port)      {};
        

        // init the listener
        int init();

        // run the listener
        int run();
    protected:
        virtual void onClientConnected(int client); // client connected
        virtual void onClientDisconnected(int client); // client disconnected
        virtual void onMessageReceived(int client, const char* msg, int length); // message is received from client

        // send message to client
        void sendToClient(int socket, const char* msg, int length);

        // broadcast message from client
        void broadcastToClients(int sending, const char* msg, int length);
    private:
        const char*     m_ip; // ip
        int             m_port; // port
        int             m_socket; // socket
        fd_set          m_master; // master file descriptor set 

};