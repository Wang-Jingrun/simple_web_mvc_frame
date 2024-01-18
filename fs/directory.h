#pragma once

#include <iostream>

#ifdef WIN32
    #include <dirent_win.h>
    #include <direct.h>
#else
    #include <dirent.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

#include <string>
#include <vector>
#include <fs/file.h>

namespace yazi
{
    namespace fs
    {
        class Directory
        {
        public:
            Directory();
            Directory(const string & path);
            ~Directory() = default;

            // 获取目录的路径
            string path() const;

            // 获取目录(含子目录)下全部文件
            std::vector<File> file() const;

            // 获取目录(含子目录)下包含多少个文件
            int count() const;

            // 获取目录(含子目录)下全部文件的行数
            int line() const;

            // 获取目录(含子目录)下全部文件的大小
            long size() const;

            // 判断目录是否存在
            bool exists() const;

            // 创建一个空目录(含子目录)
            bool create();

            // 删除目录(含子目录)
            bool remove();

            // 目录重命名
            bool rename(const string & path);

            // 清空目录(含子目录)
            void clear();

            // 复制目录
            bool copy(const string & path);

        public:
            // 获取路径分隔符: windows='\', linux='/'
            static char separator();
            static bool is_absolute_path(const string & path);
            static string normalize_path(const string & path);
            static string adjust_path(const string & path);
            static int mkdir(const char * path);
            static int rmdir(const char * path);
            static int unlink(const char * filename);
            static char * getcwd(char * buf, int len);

        private:
            string m_path;
        };
    }
}