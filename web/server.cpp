#include <web/server.h>
using namespace yazi::web;

#include <utility/string.h>
using namespace yazi::utility;

#include <reflect/class_factory.h>
using namespace yazi::reflect;

void Server::start()
{
    try
    {
        Singleton<yazi::frame::Server>::instance()->start();
    }
    catch (std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
}

void Server::bind(const string & path, server_handler handler)
{
    m_handlers[path] = handler;
}

string Server::handle(const Request & req)
{
    const string & path = req.path();
    auto it = m_handlers.find(path);
    if (it != m_handlers.end())
    {
        Response resp;
        it->second(req, resp);
        return resp.data();
    }

    if (String::has_suffix(path, ".html"))
    {
        const string & filename = get_template_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::HTML, oss.str());
            return resp.data();
        }
    }

    if (String::has_suffix(path, ".js"))
    {
        const string & filename = get_static_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::JS, oss.str());
            return resp.data();
        }
    }
    
    if (String::has_suffix(path, ".css"))
    {
        const string & filename = get_static_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::CSS, oss.str());
            return resp.data();
        }
    }

    if (String::has_suffix(path, ".jpg"))
    {
        const string & filename = get_static_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::JPG, oss.str());
            return resp.data();
        }
    }

    if (String::has_suffix(path, ".png"))
    {
        const string & filename = get_static_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::PNG, oss.str());
            return resp.data();
        }
    }

    if (String::has_suffix(path, ".gif"))
    {
        const string & filename = get_static_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::GIF, oss.str());
            return resp.data();
        }
    }

    if (String::has_suffix(path, ".ico"))
    {
        const string & filename = get_static_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::ICO, oss.str());
            return resp.data();
        }
    }

    string classname;
    string methodname;
    std::vector<string> arr = String::split(String::trim(path, " /"), '/');
    if (arr.size() == 1)
    {
        if (arr[0].empty())
        {
            classname = "Index";
            methodname = "index";
        }
        else
        {
            classname = String::capitalize(arr[0]);
            methodname = "index";
        }
    }
    else if (arr.size() == 2)
    {
        classname = String::capitalize(arr[0]);
        methodname = arr[1];
    }
    ClassFactory * factory = Singleton<ClassFactory>::instance();
    Object * ctrl = factory->create_class(classname);
    if (ctrl == nullptr)
    {
        return Response::page_not_found();
    }
    auto method = Singleton<ClassFactory>::instance()->get_class_method(classname, methodname);
    if (method == nullptr)
    {
        delete ctrl;
        return Response::page_not_found();
    }
    try
    {
        Response resp;
        ctrl->call(methodname, req, resp);
        delete ctrl;
        return resp.data();
    }
    catch (std::exception & e)
    {
        Response resp;
        resp.code(404);
        std::ostringstream oss;
        oss << "<h1 style=\"text-align: center;\">404 Page Not Found</h1><p style=\"text-align: center;\">" << e.what() << "</p>";
        resp.data(Response::HTML, oss.str());
        delete ctrl;
        return resp.data();
    }
}

void Server::set_template_folder(const string & template_folder)
{
    m_template_folder = template_folder;
}

string Server::get_template_folder() const
{
    const string & root_path = Singleton<System>::instance()->get_root_path();
    return root_path + "/" + m_template_folder;
}

void Server::set_static_folder(const string & static_folder)
{
    m_static_folder = static_folder;
}

string Server::get_static_folder() const
{
    const string & root_path = Singleton<System>::instance()->get_root_path();
    return root_path + "/" + m_static_folder;
}
