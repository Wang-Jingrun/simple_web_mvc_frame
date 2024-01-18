#include <web/file_upload.h>
using namespace yazi::web;

#include <utility/logger.h>
using namespace yazi::utility;

#include <fs/file.h>
using namespace yazi::fs;

FileUpload::FileUpload() : m_buf(nullptr), m_len(0), m_start(0), m_end(0)
{
}

FileUpload::~FileUpload()
{
}

string FileUpload::name() const
{
    return m_name;
}

string FileUpload::filename() const
{
    return m_filename;
}

string FileUpload::type() const
{
    return m_type;
}

const char * FileUpload::data() const
{
    return m_buf;
}

int FileUpload::size() const
{
    return m_end - m_start;
}

string FileUpload::extension() const
{
    return m_extension;
}

void FileUpload::parse(const char * buf, int len)
{
    m_buf = buf;
    m_len = len;

    const char * s = buf;
    const char * e = buf + len - 1;
    const char * i = s;

    // parse multipart/form-data boundary
    while ((i < e) && (strchr("\r\n", *i) == nullptr)) i++;
    m_boundary = string(s, 2, i - s);

    while ((i < e) && (strchr("\r\n", *i) != nullptr)) i++;
    s = i;

    /* parse form data headers */
    while (i < e)
    {
        // parse http form data header's name
        while ((i < e) && (strchr(": ", *i) == nullptr)) i++;
        string name = string(s, 0, i - s);

        while ((i < e) && (strchr(": ", *i) != nullptr)) i++;
        s = i;

        // parse http form data header's value
        while ((i < e) && (strchr("\r\n", *i) == nullptr)) i++;
        string value = string(s, 0, i - s);
        m_headers[name] = value;

        if (name == "Content-Disposition")
        {
            size_t pos = value.find("name=");
            size_t start = pos + 6;
            size_t end = value.find("\"", start);
            m_name = value.substr(start, end - start);
            pos = value.find("filename=");
            start = pos + 10;
            end = value.find("\"", start);
            m_filename = value.substr(start, end - start);
            size_t ext = m_filename.find(".");
            m_extension = m_filename.substr(ext);
        }
        else if (name == "Content-Type")
        {
            m_type = value;
        }

        if (strncmp(i, "\r\n\r\n", 4) == 0)
        {
            i += 4;
            break;
        }

        while ((i < e) && (strchr("\r\n", *i) != nullptr)) i++;
        s = i;
    }
    m_start = i - buf;
    string str = "--" + m_boundary + "--";
    const char * t = e - str.size();
    m_end = t - m_buf;
}

bool  FileUpload::save(const string & filename)
{
    File file(filename);
    if (!file.create())
    {
        log_error("create file error: %s", filename.c_str());
        return false;
    }

    std::ofstream ofs(filename);
    if (ofs.fail())
    {
        log_error("open file failed: %s", filename.c_str());
        return false;
    }
    ofs.write(m_buf + m_start, m_end - m_start);
    ofs.flush();
    return true;
}
