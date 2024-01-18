#pragma once

#include <map>
#include <poll.h>

namespace yazi
{
    namespace socket
    {
        class Poller
        {
        public:
            Poller();
            ~Poller();

            void create(int max_conn);
            void add(int fd);
            void del(int fd);
            int poll(int milliseconds);

            int max_fd();
            int get_fd(int idx);
            bool is_set(int idx, short event);

        private:
            struct pollfd * m_fds;
            int m_max_conn;
            int m_max_fd;
            std::map<int, int> m_fds_map;
        };
    }
}