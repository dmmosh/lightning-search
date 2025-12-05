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
        void onClientConnected(); // client connected
        void onClientDisconnected(); // client disconnected

        // send message to client
        void sendToClient(int socket, const char* msg, int length);

        // broadcast message from client
        void broadcastToClients(int sending, const char* msg, int length);
    private:
        const char*     m_ip; // ip
        uint16_t        m_port; // port
        uint16_t        m_socket; // socket
        fd_set          m_master; // master file descriptor set 

};