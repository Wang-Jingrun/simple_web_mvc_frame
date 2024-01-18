#include <web/request.h>
using namespace yazi::web;

#include <string.h>
#include <vector>

#include <utility/string.h>
#include <utility/logger.h>
using namespace yazi::utility;

int Request::parse_header(const char * buf, int len)
{
    /* parse request line: method, URI, proto */
    const char * s = buf;
    const char * e = buf + len - 1;
    const char * i = s;

    /* request is fully buffered. Skip leading whitespaces. */
    while ((i < e) && (isspace(*i))) i++;
    s = i;

    // parse http request's method
    while ((i < e) && (strchr(" ", *i) == NULL)) i++;
    m_method = string(s, 0, i - s);

    while ((i < e) && (strchr(" ", *i) != NULL)) i++;
    s = i;

    // parse http request's uri
    while ((i < e) && (strchr(" ", *i) == NULL)) i++;
    m_uri = string(s, 0, i - s);

    while ((i < e) && (strchr(" ", *i) != NULL)) i++;
    s = i;

    // parse http request's protocol
    while ((i < e) && (strchr("\r\n", *i) == NULL)) i++;
    m_proto = string(s, 0, i - s);

    while ((i < e) && (strchr("\r\n", *i) != NULL)) i++;
    s = i;

    /* if URI contains '?' character, initialize query_string */
    size_t pos = m_uri.find_first_of('?');
    if (pos != string::npos)
    {
        m_path = m_uri.substr(0, pos);
        m_query_string = m_uri.substr(pos + 1);

        std::vector<string> output = String::split(m_query_string, '&');
        for (auto it = output.begin(); it != output.end(); ++it)
        {
            std::vector<string> out = String::split((*it), '=');
            if (out.size() == 2)
            {
                m_get[out[0]] = out[1];
            }
            else
            {
                m_get[out[0]] = "";
            }
        }
    }
    else
    {
        m_path = m_uri;
    }

    /* parse request headers */
    while (i < e)
    {
        // parse http request header's name
        while ((i < e) && (strchr(": ", *i) == NULL)) i++;
        string name = string(s, 0, i - s);

        while ((i < e) && (strchr(": ", *i) != NULL)) i++;
        s = i;

        // parse http request header's value
        while ((i < e) && (strchr("\r\n", *i) == NULL)) i++;
        string value = string(s, 0, i - s);
        m_headers[name] = value;

        if (strncmp(i, "\r\n\r\n", 4) == 0)
        {
            i += 4;
            break;
        }

        while ((i < e) && (strchr("\r\n", *i) != NULL)) i++;
        s = i;
    }
    return i - buf;
}

void Request::parse_body(const char * buf, int len)
{
    if (len <= 0)
    {
        return;
    }
    m_body = buf;
    const string & content_type = header("Content-Type");
    if (content_type.find("application/json") != string::npos)
    {
        log_info("body data=\n%s", m_body.c_str());
        m_post.load(m_body.c_str(), m_body.size());
        return;
    }
    else if (content_type.find("application/x-www-form-urlencoded") != string::npos)
    {
        log_info("body data=\n%s", m_body.c_str());
        std::vector<string> output = String::split(m_body, '&');
        for (auto it = output.begin(); it != output.end(); ++it)
        {
            std::vector<string> out = String::split((*it), '=');
            if (out.size() == 2)
            {
                m_post[out[0]] = out[1];
            }
            else
            {
                m_post[out[0]] = "";
            }
        }
    }
    else if (content_type.find("multipart/form-data") != string::npos)
    {
        FileUpload upload;
        upload.parse(buf, len);
        m_files[upload.name()] = upload;
        return;
    }
}

bool Request::is_get() const
{
    return (m_method == "GET");
}

bool Request::is_post() const
{
    return (m_method == "POST");
}

string Request::get(const string & name) const
{
    auto it = m_get.find(name);
    if (it != m_get.end())
    {
        return it->second;
    }
    return "";
}

Json Request::post(const string & name) const
{
    if (name == "")
    {
        return m_post;
    }
    return m_post.get(name);
}

string Request::header(const string & name) const
{
    auto it = m_headers.find(name);
    if (it != m_headers.end())
    {
        return it->second;
    }
    return "";
}

string Request::cookie(const string & name) const
{
    auto it = m_cookies.find(name);
    if (it != m_cookies.end())
    {
        return it->second;
    }
    return "";
}

string Request::path() const
{
    return m_path;
}

string Request::user_host() const
{
    return header("Host");
}

string Request::user_agent() const
{
    return header("User-Agent");
}

int Request::content_length()
{
    return std::atoi(header("Content-Length").c_str());
}

FileUpload Request::file(const string & name) const
{
    auto it = m_files.find(name);
    if (it != m_files.end())
    {
        return it->second;
    }
    return FileUpload();
}

void Request::show() const
{
    log_debug("http method: %s", m_method.c_str());
    log_debug("http uri: %s", m_uri.c_str());
    log_debug("http proto: %s", m_proto.c_str());
    log_debug("http path: %s", m_path.c_str());
    log_debug("http query string: %s", m_query_string.c_str());
    log_debug("http headers -- start");
    for (auto it = m_headers.begin(); it != m_headers.end(); ++it)
    {
        log_debug("http header: %s=%s", it->first.c_str(), it->second.c_str());
    }
    log_debug("http headers -- end");
    log_debug("http get params -- start");
    for (auto it = m_get.begin(); it != m_get.end(); ++it)
    {
        log_debug("http get: %s=%s", it->first.c_str(), it->second.c_str());
    }
    log_debug("http get params -- end");
    log_debug("http post params -- start");
    log_debug("http post %s", m_post.str().c_str());
    log_debug("http post params -- end");
    log_debug("http body: %s", m_body.c_str());
}
