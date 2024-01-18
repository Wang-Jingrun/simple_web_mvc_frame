#pragma once

#include <map>
#include <mutex>
#include <socket/server_socket.h>
#include <socket/poller.h>

#include <utility/singleton.h>
using namespace yazi::utility;

namespace yazi
{
    namespace socket
    {
        class SocketHandler
        {
            SINGLETON(SocketHandler);
        public:
            void listen(const string & ip, int port);
            void attach(int sockfd);
            void detach(int sockfd);
            void handle(int max_conn, int timeout);   // milliseconds

        private:
            Socket * m_server = nullptr;
            Poller m_poller;
            std::mutex m_mutex;
        };
    }
}