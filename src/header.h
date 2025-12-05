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
    private:
        const char*     m_ip; // ip
        uint16_t        m_port; // port
        uint16_t        m_socket; // socket
        fd_set          m_master; // master file descriptor set 

};