#pragma once

#include <web/controller.h>
using namespace yazi::web;

namespace yazi
{
    namespace app
    {
        class Post : public Controller
        {
        public:
            void index(const Request & req, Response & resp);
            void json(const Request & req, Response & resp);
        };
    }
}