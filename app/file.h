#pragma once

#include <web/controller.h>
using namespace yazi::web;

namespace yazi
{
    namespace app
    {
        class File : public Controller
        {
        public:
            void index(const Request & req, Response & resp);
            void upload(const Request & req, Response & resp);
        };
    }
}
