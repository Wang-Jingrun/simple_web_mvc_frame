#include <fs/directory.h>
using namespace yazi::fs;

Directory::Directory()
{
    m_path = normalize_path(".");
}

Directory::Directory(const string & path)
{
    m_path = normalize_path(path);
}

string Directory::path() const
{
    return m_path;
}

std::vector<File> Directory::file() const
{
    std::vector<File> files;
    DIR * dir = opendir(m_path.c_str());
    if (dir == nullptr)
    {
        return files;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        string filename = entry->d_name;
        if (filename == "." || filename == "..")
        {
            continue;
        }
        string fullname = m_path + separator() + filename;

        struct stat info = {0};
        if (stat(fullname.c_str(), &info) != 0)
        {
            std::cerr << "can not stat file: " << fullname << std::endl;
            return files;
        }
        if (S_ISDIR(info.st_mode))
        {
            // 目录
            Directory tmp(fullname);
            auto vec = tmp.file();
            files.insert(files.end(), vec.begin(), vec.end());
        }
        else
        {
            // 文件
            files.push_back(File(fullname));
        }
    }
    return files;
}

int Directory::count() const
{
    auto files = file();
    return (int)files.size();
}

int Directory::line() const
{
    int line = 0;
    auto files = file();
    for (auto & file : files)
    {
        line += file.line();
    }
    return line;
}

long Directory::size() const
{
    long size = 0;
    auto files = file();
    for (auto & file : files)
    {
        size += file.size();
    }
    return size;
}

bool Directory::exists() const
{
    if (m_path.empty())
    {
        return false;
    }
    struct stat info = {0};
    if (stat(m_path.c_str(), &info) != 0)
    {
        return false;
    }
    if (info.st_mode & S_IFDIR)
    {
        return true;
    }
    return false;
}

bool Directory::create()
{
    char sep = separator();
    std::vector<string> arr = String::split(m_path, sep);

    string path;
    for (const auto & one : arr)
    {
        if (one.empty())
        {
            continue;
        }
        if (path.empty())
        {
            path += one;
        }
        else
        {
            path += sep + one;
        }
        path = adjust_path(path);
        Directory dir(path);
        if (dir.exists())
        {
            continue;
        }
        if (mkdir(path.c_str()) != 0)
        {
            return false;
        }
    }
    return true;
}

bool Directory::remove()
{
    DIR * dir = opendir(m_path.c_str());
    if (dir == nullptr)
    {
        return false;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        string filename = entry->d_name;
        if (filename == "." || filename == "..")
        {
            continue;
        }
        string fullname = m_path + separator() + filename;

        struct stat info = {0};
        if (stat(fullname.c_str(), &info) != 0)
        {
            std::cerr << "stat file error: " << fullname << std::endl;
            return false;
        }
        if (S_ISDIR(info.st_mode))
        {
            // 目录
            Directory tmp(fullname);
            tmp.remove();
        }
        else
        {
            // 删除文件
            unlink(fullname.c_str());
        }
    }
    rmdir(m_path.c_str());
    return true;
}

bool Directory::rename(const string & path)
{
    if (std::rename(m_path.c_str(), path.c_str()) != 0)
    {
        return false;
    }
    m_path = normalize_path(path);
    return true;
}

void Directory::clear()
{
    DIR * dir = opendir(m_path.c_str());
    if (dir == nullptr)
    {
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        string filename = entry->d_name;
        if (filename == "." || filename == "..")
        {
            continue;
        }
        string fullname = m_path + separator() + filename;

        struct stat info = {0};
        if (stat(fullname.c_str(), &info) != 0)
        {
            std::cerr << "stat file error: " << fullname << std::endl;
            return;
        }
        if (S_ISDIR(info.st_mode))
        {
            // 目录
            Directory tmp(fullname);
            tmp.remove();
        }
        else
        {
            // 删除文件
            unlink(fullname.c_str());
        }
    }
}

bool Directory::copy(const string & path)
{
    Directory dir(path);
    if (!dir.exists())
    {
        if (!dir.create())
        {
            return false;
        }
    }
    auto files = file();
    for (auto & file : files)
    {
        string src = file.path();
        string dst = dir.path() + src.substr(m_path.length());
        if (!file.copy(dst))
        {
            return false;
        }
    }
    return true;
}

char Directory::separator()
{
#ifdef WIN32
    return '\\';
#else
    return '/';
#endif
}

bool Directory::is_absolute_path(const string & path)
{
    if (path.empty())
    {
        return false;
    }
    char sep = Directory::separator();
    string filepath = path;

#ifdef WIN32
    std::replace(filepath.begin(), filepath.end(), '/', sep);
#endif

    std::vector<string> output = String::split(filepath, sep);

#ifdef WIN32
    if (output[0].find(':') != std::string::npos)
#else
    if (output[0].empty())
#endif
    {
        return true;
    }
    else
    {
        return false;
    }
}

string Directory::normalize_path(const string & path)
{
    char sep = Directory::separator();
    string filepath = path;

#ifdef WIN32
    std::replace(filepath.begin(), filepath.end(), '/', sep);
#endif

    std::vector<string> path_list;
    if (!is_absolute_path(path))
    {
        char cwd[PATH_MAX] = {0};
        getcwd(cwd, PATH_MAX);
        path_list = String::split(cwd, sep);
    }
    for (const auto & dir : String::split(filepath, sep))
    {
        if (dir.empty())
        {
            continue;
        }
        if (dir == ".")
        {
            continue;
        }
        else if (dir == "..")
        {
            path_list.pop_back();
        }
        else
        {
            path_list.push_back(dir);
        }
    }
    string temp = String::join(path_list, sep);
    return adjust_path(temp);
}

string Directory::adjust_path(const string & path)
{
    string result = path;
    char sep = separator();
#ifdef WIN32
    if (path[path.size() - 1] == ':')
    {
        result += sep;
    }
#else
    if (result[0] != sep)
    {
        result = string(1, sep) + result;
    }
#endif
    return result;
}

int Directory::mkdir(const char *path)
{
#ifdef WIN32
    return ::_mkdir(path);
#else
    return ::mkdir(path, 0755);
#endif
}

int Directory::rmdir(const char * path)
{
#ifdef WIN32
    return ::_rmdir(path);
#else
    return ::rmdir(path);
#endif
}

int Directory::unlink(const char * filename)
{
#ifdef WIN32
    return ::_unlink(filename);
#else
    return ::unlink(filename);
#endif
}

char * Directory::getcwd(char * buf, int len)
{
#ifdef WIN32
    return ::_getcwd(buf, len);
#else
    return ::getcwd(buf, len);
#endif
}
