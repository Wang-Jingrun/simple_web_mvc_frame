#pragma once

#include <map>
#include <string>
using std::string;

#include <json/json.h>
using namespace yazi::json;

#include <web/file_upload.h>

namespace yazi
{
    namespace web
    {
        class Request
        {
        public:
            Request() = default;
            ~Request() = default;

            int parse_header(const char * buf, int len);
            void parse_body(const char * buf, int len);

            bool is_get() const;
            bool is_post() const;

            string get(const string & name) const;
            Json post(const string & name = "") const;
            string header(const string & name) const;
            string cookie(const string & name) const;
            FileUpload file(const string & name) const;
            string path() const;
            string user_agent() const;
            string user_host() const;

            int content_length();

            void show() const;

        private:
            string m_method;
            string m_uri;
            string m_proto;
            string m_path;
            string m_body;
            string m_query_string;
            std::map<string, string> m_get;
            Json m_post;
            std::map<string, string> m_headers;
            std::map<string, string> m_cookies;
            std::map<string, FileUpload> m_files;
        };
    }
}
