#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <sstream>

using namespace std;

int main()
{
    // Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return -1;
    }

    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.s_addr = INADDR_ANY; // Could also use inet_pton ....

    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1)
    {
        cerr << "Can't bind to IP/port! Quitting" << endl;
        return -2;
    }

    // Tell the socket to listen
    if (listen(listening, SOMAXCONN) == -1)
    {
        cerr << "Can't listen! Quitting" << endl;
        return -3;
    }

    // Create the master file descriptor set and zero it
    fd_set master;
    FD_ZERO(&master);

    // Add our first socket that we're interested in interacting with; the listening socket!
    FD_SET(listening, &master);

    bool running = true;

    while (running)
    {
        fd_set copy = master;

        int socketCount = select(FD_SETSIZE, &copy, nullptr, nullptr, nullptr);

        for (int i = 0; i < socketCount; i++)
        {
            int sock = copy.fds_bits[i];

            if (sock == listening)
            {
                int client = accept(listening, nullptr, nullptr);

                FD_SET(client, &master);

                string welcomeMsg = "Welcome to the Awesome Chat Server!\n";
                send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
            }
            else
            {
                char buf[4096];
                memset(buf, 0, 4096);

                int bytesIn = recv(sock, buf, 4096, 0);
                if (bytesIn <= 0)
                {
                    close(sock);
                    FD_CLR(sock, &master);
                }
                else
                {
                    if (buf[0] == '\\')
                    {
                        string cmd = string(buf, bytesIn);
                        if (cmd == "\\quit")
                        {
                            running = false;
                            break;
                        }

                        continue;
                    }

                    for (int i = 0; i < FD_SETSIZE; i++)
                    {
                        int outSock = master.fds_bits[i];
                        if (outSock != listening && outSock != sock)
                        {
                            ostringstream ss;
                            ss << "SOCKET #" << sock << ": " << buf << "\n";
                            string strOut = ss.str();

                            send(outSock, strOut.c_str(), strOut.size() + 1, 0);
                        }
                    }
                }
            }
        }
    }

    FD_CLR(listening, &master);
    close(listening);

    string msg = "Server is shutting down. Goodbye\n";

    for (int i = 0; i < FD_SETSIZE; i++)
    {
        int sock = master.fds_bits[i];

        send(sock, msg.c_str(), msg.size() + 1, 0);

        FD_CLR(sock, &master);
        close(sock);
    }

    return 0;
}