#include <web/response.h>
#include <web/server.h>
using namespace yazi::web;

Response::Response() : m_code(200)
{
}

Response::~Response()
{
}

void Response::code(int code)
{
    m_code = code;
}

void Response::data(Type type, const string & data)
{
    m_type = type;
    m_data = data;
}

string Response::data() const
{
    std::ostringstream oss;
    oss << "HTTP/1.1 " << m_code << " OK\r\n";
    switch (m_type)
    {
        case HTML:
            oss << "Content-Type: text/html; charset: utf-8\r\n";
            break;
        case JSON:
            oss << "Content-Type: application/json; charset: utf-8\r\n";
            break;
        case JS:
            oss << "Content-Type: text/javascript; charset: utf-8\r\n";
            break;
        case CSS:
            oss << "Content-Type: text/css; charset: utf-8\r\n";
            break;
        case JPG:
            oss << "Content-Type: image/jpeg; charset: utf-8\r\n";
            break;
        case PNG:
            oss << "Content-Type: image/png; charset: utf-8\r\n";
            break;
        case GIF:
            oss << "Content-Type: image/gif; charset: utf-8\r\n";
            break;
        case ICO:
            oss << "Content-Type: image/x-icon; charset: utf-8\r\n";
            break;
        default:
            break;
    }
    oss << "Content-Length: " << m_data.size() << "\r\n\r\n";
    oss << m_data << "\r\n";
    return oss.str();
}

void Response::html(const string & data)
{
    m_type = HTML;
    m_data = data;
}

void Response::json(const string & data)
{
    m_type = JSON;
    m_data = data;
}

string Response::page_not_found()
{
    string body = "<!DOCTYPE html> \
<html> \
    <head> \
        <meta charset=\"utf-8\"> \
        <title>yazi-web</title> \
    </head> \
    <body> \
        <h1 style=\"text-align: center;\">404 Page Not Found</h1> \
        <p style=\"text-align: center;\">request not matched</p> \
    </body> \
</html>";

    std::ostringstream oss;
    oss << "HTTP/1.1 404 Not Found\r\n";
    oss << "Content-Type: text/html; charset: UTF-8\r\n";
    oss << "Content-Length: " << body.size() << "\r\n\r\n";
    oss << body << "\r\n";
    return oss.str();
}

void Response::render(const string & filename)
{
    const string & template_folder = Singleton<Server>::instance()->get_template_folder();
    const string & file = template_folder + "/" + filename;
    std::ifstream ifs(file);
    if (ifs.fail())
    {
        return;
    }
    std::ostringstream oss;
    oss << ifs.rdbuf();
    m_type = HTML;
    m_data = oss.str();
}
