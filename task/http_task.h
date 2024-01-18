#pragma once

#include <vector>

#include <socket/socket.h>
using namespace yazi::socket;

#include <thread/task.h>
using namespace yazi::thread;

#include <web/request.h>
using namespace yazi::web;

namespace yazi
{
    namespace task
    {
        const uint32_t recv_buff_size = 1024 * 8;

        class HttpTask : public Task
        {
        public:
            HttpTask(int sockfd);
            ~HttpTask();

            void reset();

            virtual void run();
            virtual void destroy();

        private:
            int m_sockfd;
            bool m_closed;
            Request m_req;
            int m_total_len;
            int m_head_len;
            int m_body_len;
            int m_body_idx;
            char * m_body;
        };
    }
}
