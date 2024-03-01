#include <socket/socket.h>
using namespace yazi::socket;

Socket::Socket() : m_ip(""), m_port(0), m_sockfd(0)
{
	// IPv4	传输层使用TCP协议
	m_sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sockfd < 0) // 套接字创建失败
		log_error("create socket error: errno=%d errmsg=%s", errno, strerror(errno));
	else
		log_debug("create socket success!");
}

Socket::Socket(int sockfd) : m_ip(""), m_port(0), m_sockfd(sockfd)
{
}

Socket::~Socket()
{
}

int Socket::fd() const
{
	return m_sockfd;
}

bool Socket::bind(const string& ip, int port)
{
	struct sockaddr_in sockaddr; // IPv4 协议族对应的结构体
	std::memset(&sockaddr, 0, sizeof(sockaddr)); // 参数重置为 0
	sockaddr.sin_family = AF_INET; // IPv4

	if (ip.empty())
		sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY 表示任意的IP地址
	else
		// 将点分十进制形式的 IP 地址转换为32位无符号整数形式的网络字节序（大端字节序）的IP地址
		sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());

	sockaddr.sin_port = htons(port); // 使用网络统一的编码

	if (::bind(m_sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) // 如果绑定失败
	{
		log_error("socket bind error: errno=%d, errmsg=%s", errno, strerror(errno));
		return false;
	}

	// 绑定成功
	m_ip = ip;
	m_port = port;
	log_debug("socket bind success: ip=%s, port=%d", ip.c_str(), port);
	return true;
}

bool Socket::listen(int backlog)
{
	if (::listen(m_sockfd, backlog) < 0) // 设置监听失败
	{
		log_error("socket listen error: errno=%d errmsg=%s", errno, strerror(errno));
		return false;
	}

	log_debug("socket listening ...");
	return true;
}

bool Socket::connect(const string& ip, int port)
{
	struct sockaddr_in sockaddr; // IPv4 协议族对应的结构体
	std::memset(&sockaddr, 0, sizeof(sockaddr)); // 参数重置为 0
	sockaddr.sin_family = AF_INET; // IPv4
	sockaddr.sin_addr.s_addr = inet_addr(ip.c_str()); // 将点分十进制形式的 IP 地址转换为32位无符号整数形式的网络字节序（大端字节序）的IP地址
	sockaddr.sin_port = htons(port); // 使用网络统一的编码

	if (::connect(m_sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) // 连接失败
	{
		log_error("socket connect error: errno=%d errmsg=%s", errno, strerror(errno));
		return false;
	}

	// 连接成功
	m_ip = ip;
	m_port = port;
	return true;
}

int Socket::accept()
{
	int connfd = ::accept(m_sockfd, nullptr, nullptr);
	if (connfd < 0) // 接收失败
	{
		log_error("socket accept error: errno=%d errmsg=%s", errno, strerror(errno));
		return connfd;
	}

	log_debug("socket accept: conn=%d", connfd);
	return connfd;
}

int Socket::send(const char* buf, int len)
{
	return ::send(m_sockfd, buf, len, 0);
}

int Socket::recv(char* buf, int len)
{
	return ::recv(m_sockfd, buf, len, 0);
}

void Socket::close()
{
	if (m_sockfd > 0)
	{
		::close(m_sockfd);
		m_sockfd = 0;
	}
}

// 设置非阻塞模式，提高程序的并发性和响应能力
bool Socket::set_non_blocking()
{
	int flags = fcntl(m_sockfd, F_GETFL, 0);
	if (flags < 0)
	{
		log_error("socket set_non_blocking error: errno=%d errmsg=%s", errno, strerror(errno));
		return false;
	}
	flags |= O_NONBLOCK;
	if (fcntl(m_sockfd, F_SETFL, flags) < 0)
	{
		log_error("Socket::set_non_blocking(F_SETFL, O_NONBLOCK): errno=%d errstr=%s", errno, strerror(errno));
		return false;
	}
	return true;
}

// 设置套接字的发送缓冲区大小
bool Socket::set_send_buffer(int size)
{
	int buff_size = size;
	if (setsockopt(m_sockfd, SOL_SOCKET, SO_SNDBUF, &buff_size, sizeof(buff_size)) < 0)
	{
		log_error("socket set_send_buffer error: errno=%d errmsg=%s", errno, strerror(errno));
		return false;
	}
	return true;
}

// 设置套接字的接收缓冲区大小
bool Socket::set_recv_buffer(int size)
{
	int buff_size = size;
	if (setsockopt(m_sockfd, SOL_SOCKET, SO_RCVBUF, &buff_size, sizeof(buff_size)) < 0)
	{
		log_error("socket set_recv_buffer error: errno=%d errmsg=%s", errno, strerror(errno));
		return false;
	}
	return true;
}

// 设置套接字的延迟关闭选项
// 	默认情况下，当调用close函数关闭套接字时，操作系统会立即
// 关闭套接字并丢弃发送缓冲区中的未发送数据。但是，在某些情况
// 下，可能希望等待一段时间，让套接字将发送缓冲区的数据发送完
// 毕，再关闭套接字。
bool Socket::set_linger(bool active, int seconds)
{
	struct linger l;
	std::memset(&l, 0, sizeof(l));
	l.l_onoff = active ? 1 : 0;
	l.l_linger = seconds;

	if (setsockopt(m_sockfd, SOL_SOCKET, SO_LINGER, &l, sizeof(l)) < 0)
	{
		log_error("socket set_linger error: errno=%d errmsg=%s", errno, strerror(errno));
		return false;
	}
	return true;
}

bool Socket::set_keepalive()
{
	int flag = 1;
	if (setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag)) < 0)
	{
		log_error("socket set_keep_alive error: errno=%d errmsg=%s", errno, strerror(errno));
		return false;
	}
	return true;
}

// 设置套接字的TCP Keep-Alive选项
// 	TCP Keep-Alive 用于检测在长时间空闲之后是否断开了TCP连接。
// 通过启用Keep-Alive选项，可以使操作系统定期发送探测报文到对端，以检测连接的有效性。
bool Socket::set_reuseaddr()
{
	int flag = 1;
	if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0)
	{
		log_error("socket set_reuse_addr error: errno=%d errmsg=%s", errno, strerror(errno));
		return false;
	}
	return true;
}
