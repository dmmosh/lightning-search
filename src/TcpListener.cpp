#include "header.h"
#include <iostream>
#include <string.h>     // For memset
#include <unistd.h>     // For close()
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

// Define generic types for portability
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

int TcpListener::init()
{
    // NO WSAStartup needed on Linux

    // Create a socket
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == INVALID_SOCKET)
    {
        return -1; // Standard Linux error is -1
    }

    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(m_port);
    inet_pton(AF_INET, m_ip, &hint.sin_addr);

    // Set socket option to reuse address (prevents "Address already in use" errors on restart)
    int opt = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(m_socket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
    {
        return -1;
    }

    // Tell the socket is for listening
    if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        return -1;
    }

    // Create the master file descriptor set and zero it
    FD_ZERO(&m_master);

    // Add our listening socket
    FD_SET(m_socket, &m_master);

    // Keep track of the biggest file descriptor for select()
    m_max_fd = m_socket;

    return 0;
}

int TcpListener::run()
{
    bool running = true;

    while (running)
    {
        // Make a copy of the master file descriptor set
        fd_set copy = m_master;

        // See who's talking to us
        // Linux requires the first param to be max_fd + 1
        int socketCount = select(m_max_fd + 1, &copy, nullptr, nullptr, nullptr);

        if (socketCount == -1) {
            // Error handling
            break;
        }

        // Linux Loop: We must iterate from 0 to m_max_fd to find which sockets are ready
        // We cannot use fd_array or fd_count like in Windows
        for (int i = 0; i <= m_max_fd; i++)
        {
            // Check if this socket descriptor is in the 'copy' set
            if (FD_ISSET(i, &copy))
            {
                SOCKET sock = i; // The current socket descriptor

                // Is it the listening socket?
                if (sock == m_socket)
                {
                    // Accept a new connection
                    SOCKET client = accept(m_socket, nullptr, nullptr);

                    if (client != INVALID_SOCKET) {
                        // Add the new connection to the list of connected clients
                        FD_SET(client, &m_master);

                        // Update max_fd if this new socket ID is higher
                        if (client > m_max_fd) {
                            m_max_fd = client;
                        }

                        onClientConnected(client);
                    }
                }
                else // It's an inbound message from a client
                {
                    char buf[4096];
                    memset(buf, 0, 4096); // Linux replacement for ZeroMemory

                    // Receive message
                    int bytesIn = recv(sock, buf, 4096, 0);
                    if (bytesIn <= 0)
                    {
                        // Drop the client
                        onClientDisconnected(sock);
                        close(sock); // Linux uses close(), not closesocket()
                        FD_CLR(sock, &m_master);
                    }
                    else
                    {
                        onMessageReceived(sock, buf, bytesIn);
                    }
                }
            }
        }
    }

    // Clean up all sockets
    // Linux cleanup loop
    for (int i = 0; i <= m_max_fd; i++) {
        if (FD_ISSET(i, &m_master)) {
            close(i);
        }
    }

    // No WSACleanup needed
    return 0;
}

void TcpListener::sendToClient(int clientSocket, const char* msg, int length)
{
    // recv/send on Linux are nearly identical, but strict types might require void* or char* casts depending on flags
    send(clientSocket, msg, length, 0);
}

void TcpListener::broadcastToClients(int sendingClient, const char* msg, int length)
{
    // We cannot loop through fd_array on Linux. 
    // We iterate up to m_max_fd and check if they are in the master set.
    for (int i = 0; i <= m_max_fd; i++)
    {
        if (FD_ISSET(i, &m_master))
        {
            SOCKET outSock = i;
            if (outSock != m_socket && outSock != sendingClient)
            {
                sendToClient(outSock, msg, length);
            }
        }
    }
}

void TcpListener::onClientConnected(int clientSocket)
{
    // Implementation left to user
}

void TcpListener::onClientDisconnected(int clientSocket)
{
    // Implementation left to user
}

void TcpListener::onMessageReceived(int clientSocket, const char* msg, int length)
{
    // Implementation left to user
}