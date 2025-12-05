#include "header.h"



int TcpListener::init(){
    // Create a socket
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == -1)
    {
        std::cerr << "Can't create a socket! Quitting\t" << strerror(errno) << '\n';
        return -1;
    }

    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(m_port);
    inet_pton(AF_INET, m_ip, &hint.sin_addr); // converts human-readable ips to compact binary format 
    // inet_ntop: converts binary back to text

    if (bind(m_socket, (sockaddr*)&hint, sizeof(hint)) == -1)
    {
        std::cerr << "Can't bind to IP/port! Quitting\t" << strerror(errno) << '\n';
        return -2;
    }

    // Tell the socket to listen
    if (listen(m_socket, SOMAXCONN) == -1)
    {
        std::cerr << "Can't listen! Quitting\t" << strerror(errno) << '\n';
        return -3;
    }

    // Create the master file descriptor set and zero it
    FD_ZERO(&m_master);

    // Add our first socket that we're interested in interacting with; the listening socket!
    FD_SET(m_socket, &m_master);

    return 0;
}


int TcpListener::run(){
    bool running = true;

    while (running)
    {
        fd_set copy = m_master;

        int socketCount = select(FD_SETSIZE, &copy, nullptr, nullptr, nullptr);

        for (int i = 0; i < socketCount; i++)
        {
            int sock = copy.fds_bits[i];

            if (sock == m_socket) // CLIENT CONNECTED
            {
                int client = accept(m_socket, nullptr, nullptr);

                FD_SET(client, &m_master);

                onClientConnected(client);
                // std::string welcomeMsg = "Welcome to the Awesome Chat Server!\n";
                // send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
            }
            else // CLIENT DISCONNECTED 
            {
                char buf[4096];
                memset(buf, 0, 4096);

                int bytesIn = recv(sock, buf, 4096, 0);
                if (bytesIn <= 0)
                {
                    onClientDisconnected(sock);
                    close(sock);
                    FD_CLR(sock, &m_master);
                }
                else
                {
                    onMessageReceived(sock, buf, bytesIn);
                    // if (buf[0] == '\\')
                    // {
                    //     std::string cmd = std::string(buf, bytesIn);
                    //     if (cmd == "\\quit")
                    //     {
                    //         running = false;
                    //         break;
                    //     }

                    //     continue;
                    // }

                    // for (int i = 0; i < FD_SETSIZE; i++)
                    // {
                    //     int outSock = m_master.fds_bits[i];
                    //     if (outSock != m_socket && outSock != sock)
                    //     {
                    //         // ostringstream ss;
                    //         // ss << "SOCKET #" << sock << ": " << buf << "\n";
                    //         // string strOut = ss.str();

                    //         // send(outSock, strOut.c_str(), strOut.size() + 1, 0);
                    //     }
                    // }
                }
            }
        }
    }

    FD_CLR(m_socket, &m_master);
    close(m_socket);

    // std::string msg = "Server is shutting down. Goodbye\n";

    for (int i = 0; i < FD_SETSIZE; i++)
    {
        int sock = m_master.fds_bits[i];
        // send(sock, "bye bye", 9, 0);

        FD_CLR(sock, &m_master);
        close(sock);
    }

    return 0;

}

        // send message to client
void TcpListener::sendToClient(int socket, const char* msg, int length){
    send(socket, msg, length, 0);
};
    // broadcast message from client
void TcpListener::broadcastToClients(int sending, const char* msg, int length){
    for (int i = 0; i < FD_SETSIZE; i++)
                    {
                        int outSock = m_master.fds_bits[i];
                        if (outSock != m_socket && outSock != sending)
                        {
                            sendToClient(outSock, msg, length);
                        }
                    }
};

void TcpListener::onClientConnected(int client){ // client connected


};

void TcpListener::onClientDisconnected(int client){ // client disconnected


};


void TcpListener::onMessageReceived(int client, const char* msg, int length){ // message is received from client


};


