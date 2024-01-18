#pragma once

#include <fstream>
#include <sstream>
#include <functional>
#include <string>
using std::string;

#include <frame/server.h>

#include <web/request.h>
#include <web/response.h>

namespace yazi
{
    namespace web
    {
        typedef std::function<void(const Request &, Response &)> server_handler;

        class Server
        {
            SINGLETON(Server);
        public:
            void start();

            void bind(const string & path, server_handler handler);
            string handle(const Request & req);

            void set_template_folder(const string & template_folder);
            string get_template_folder() const;

            void set_static_folder(const string & static_folder);
            string get_static_folder() const;

        private:
            std::map<string, server_handler> m_handlers;
            string m_template_folder = "template";
            string m_static_folder = "static";
        };
    }
}
