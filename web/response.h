#pragma once

#include <fstream>
#include <sstream>
#include <string>
using std::string;

namespace yazi
{
    namespace web
    {
        class Response
        {
        public:
            enum Type
            {
                HTML = 0,
                JSON,
                JS,
                CSS,
                JPG,
                PNG,
                GIF,
                ICO
            };

            Response();
            ~Response();

            void code(int code);
            void data(Type type, const string & data);
            string data() const;

            void html(const string & data);
            void json(const string & data);

            static string page_not_found();
            void render(const string & filename);

        private:
            int m_code;
            Type m_type;
            string m_data;
        };
    }
}
