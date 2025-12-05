#pragma once
#include <cstdint>



class TcpListener{
    public:

        // init the listener
        int init();
        int run();
    protected:

    private:
        const char*     ip;
        uint16_t        port;
        uint16_t         socket;
};