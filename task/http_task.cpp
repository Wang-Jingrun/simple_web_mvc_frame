#include <task/http_task.h>
#include <task/task_factory.h>
using namespace yazi::task;

#include <socket/socket_handler.h>
using namespace yazi::socket;

#include <web/server.h>
using namespace yazi::web;


HttpTask::HttpTask(int sockfd) : Task(), m_sockfd(sockfd)
{
    m_total_len = 0;
    m_head_len = 0;
    m_body_len = 0;
    m_body_idx = 0;
    m_body = nullptr;
}

HttpTask::~HttpTask()
{
}

void HttpTask::reset()
{
    m_total_len = 0;
    m_head_len = 0;
    m_body_len = 0;
    m_body_idx = 0;
    if (m_body != nullptr)
    {
        delete [] m_body;
        m_body = nullptr;
    }
}

void HttpTask::run()
{
    log_debug("http task run: conn=%d", m_sockfd);

    // 接收客户端的数据
    char buf[recv_buff_size] = {0};
    Socket socket(m_sockfd);
    int len = socket.recv(buf, sizeof(buf));
    if (len < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            log_debug("socket recv would block: conn=%d", m_sockfd);
            return;
        }
        else if (errno == EINTR)
        {
            log_debug("socket recv interrupted: conn=%d", m_sockfd);
            return;
        }
        log_debug("socket connection abort: conn=%d", m_sockfd);
        m_closed = true;
        return;
    }
    else if (len == 0)
    {
        log_debug("socket closed by peer: conn=%d", m_sockfd);
        m_closed = true;
        return;
    }

    log_debug("recv: conn=%d msg=\n%s", m_sockfd, buf);

    m_total_len += len;
    if (m_head_len == 0)
    {
        m_head_len = m_req.parse_header(buf, len);
        m_body_len = m_req.content_length();
        // log_info("recv head size=%d, header-length=%d, content-length=%d", len, m_head_len, m_body_len);
        if (m_body_len > 0)
        {
            m_body = new char[m_body_len + 1];
            m_body[m_body_len] = 0;
            std::memcpy(m_body, buf + m_head_len, len - m_head_len);
            m_body_idx = len - m_head_len;
        }
    }
    else
    {
        std::memcpy(m_body + m_body_idx, buf, len);
        m_body_idx += len;
    }
    if (m_total_len - m_head_len >= m_body_len)
    {
        // log_info("recv body complete: body-len=%d, body-data=\n%s", m_body_idx, m_body);
        m_req.parse_body(m_body, m_body_len);
        auto server = Singleton<Server>::instance();
        string output = server->handle(m_req);
        socket.send(output.c_str(), output.size());
        reset();
    }
}

void HttpTask::destroy()
{
    log_debug("http task destroy: conn=%d", m_sockfd);
    if (m_closed)
    {
        Singleton<TaskFactory>::instance()->remove(m_sockfd);
    }
    else
    {
        Singleton<SocketHandler>::instance()->attach(m_sockfd);
    }
}
