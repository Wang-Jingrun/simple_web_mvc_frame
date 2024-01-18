#pragma once

#include <web/controller.h>
using namespace yazi::web;

namespace yazi
{
    namespace app
    {
        class Index : public Controller
        {
        public:
            void index(const Request & req, Response & resp);
            void show(const Request & req, Response & resp);
        };
    }
}
