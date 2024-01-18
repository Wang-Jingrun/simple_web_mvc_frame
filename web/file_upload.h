#pragma once

#include <map>
#include <cstring>
#include <string>
using std::string;

namespace yazi
{
    namespace web
    {
        class FileUpload
        {
        public:
            FileUpload();
            ~FileUpload();

            string name() const;
            string filename() const;
            string type() const;
            const char * data() const;
            int size() const;
            string extension() const;

            void parse(const char * buf, int len);
            bool save(const string & filename);

        private:
            string m_name;
            string m_type;
            string m_filename;
            string m_extension;
            string m_boundary;
            std::map<string, string> m_headers;
            const char * m_buf;
            int m_len;
            int m_start;
            int m_end;
        };
    }
}
