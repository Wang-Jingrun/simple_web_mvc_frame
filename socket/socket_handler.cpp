#include <socket/socket_handler.h>
using namespace yazi::socket;

#include <task/task_factory.h>
using namespace yazi::task;

#include <thread/task_dispatcher.h>
using namespace yazi::thread;

void SocketHandler::listen(const string &ip, int port)
{
    m_server = new ServerSocket(ip, port);
}

void SocketHandler::attach(int sockfd)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_poller.add(sockfd);
}

void SocketHandler::detach(int sockfd)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_poller.del(sockfd);
}

void SocketHandler::handle(int max_conn, int timeout)
{
    m_poller.create(max_conn);

    // 把服务端套接字加入队列里
    attach(m_server->fd());

    while (true)
    {
        int num = m_poller.poll(timeout); // -1 表示一直等待
        if (num < 0)
        {
            log_error("poll error: errno=%d errmsg=%s", errno, strerror(errno));
            break;
        }
        else if (num == 0)
        {
            log_debug("poll timeout");
            continue;
        }
        for (int i = 0; i < m_poller.max_fd() + 1; i++)
        {
            if (!m_poller.is_set(i, POLLIN | POLLHUP | POLLERR))
            {
                continue;
            }

            if (num-- == 0)
            {
                break;
            }

            if (i == 0)
            {
                // 服务端套接字可读
                int connfd = m_server->accept();
                if (connfd < 0)
                {
                    log_error("server accept error: errno=%d errmsg=%s", errno, strerror(errno));
                }
                else
                {
                    // 把连接套接字加入到监听队列中
                    Socket socket(connfd);
                    socket.set_non_blocking();
                    attach(connfd);
                }
            }
            else
            {
                // 连接套接字可读
                int connfd = m_poller.get_fd(i);
                detach(connfd);

                if (m_poller.is_set(i, POLLHUP))
                {
                    // 连接套接字挂断了
                    log_error("socket hang up by peer: conn=%d", connfd);
                    ::close(connfd);
                }
                else if (m_poller.is_set(i, POLLERR))
                {
                    // 连接发生错误
                    log_error("socket error: conn=%d", connfd);
                    ::close(connfd);
                }
                else if (m_poller.is_set(i, POLLIN))
                {
                    auto task = Singleton<TaskFactory>::instance()->create(connfd);
                    Singleton<TaskDispatcher>::instance()->assign(task);
                }
            }
        }
    }
}