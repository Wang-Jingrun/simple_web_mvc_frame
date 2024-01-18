#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <climits>
#include <vector>
#include <string>
using std::string;

#include <utility/string.h>
using namespace yazi::utility;

namespace yazi
{
    namespace fs
    {
        class File
        {
        public:
            File() = delete;
            File(const string & path);
            ~File() = default;

            // 获取文件路径
            string path() const;

            // 获取文件所在文件夹路径
            string dir() const;

            // 创建一个空文件
            bool create();

            // 删除文件
            bool remove();

            // 修改文件名
            bool rename(const string & path);

            // 清空文件
            void clear();

            // 判断文件是否存在
            bool exists() const;

            // 复制文件
            bool copy(const string & path);

            // 获取文件的行数
            int line() const;

            // 获取文件的大小
            long size() const;

            // 获取文件最后的修改时间
            time_t time() const;

            // 读取文件全部内容
            string read() const;

            // 写入文件
            bool write(const string & data);

        private:
            string m_path;
        };
    }
}