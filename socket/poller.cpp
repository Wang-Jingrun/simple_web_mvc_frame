#include <socket/poller.h>
using namespace yazi::socket;

Poller::Poller() : m_fds(nullptr), m_max_conn(0), m_max_fd(0)
{
}

Poller::~Poller()
{
    if (m_fds != nullptr)
    {
        delete [] m_fds;
        m_fds = nullptr;
    }
}

void Poller::create(int max_conn)
{
    m_fds = new pollfd[max_conn];
    for (int i = 0; i < max_conn; i++)
    {
        m_fds[i].fd = -1;
    }
    m_max_conn = max_conn;
}

void Poller::add(int fd)
{
    // 把连接套接字加入到监听队列中
    for (int i = 0; i < m_max_conn; i++)
    {
        if (m_fds[i].fd == -1) // 找到空闲的 poll fd
        {
            m_fds[i].fd = fd;
            m_fds[i].events = POLLIN | POLLHUP | POLLERR;
            m_fds_map[fd] = i;

            if (m_max_fd < i)
            {
                m_max_fd = i;
            }
            return;
        }
    }
}

void Poller::del(int fd)
{
    auto it = m_fds_map.find(fd);
    if (it == m_fds_map.end())
    {
        return;
    }
    m_fds[it->second].fd = -1;
}

bool Poller::is_set(int idx, short events)
{
    return m_fds[idx].revents & events;
}

int Poller::poll(int milliseconds)
{
    return ::poll(m_fds, m_max_fd + 1, milliseconds);
}

int Poller::max_fd()
{
    return m_max_fd;
}

int Poller::get_fd(int idx)
{
    return m_fds[idx].fd;
}