#pragma once

#include <web/controller.h>
using namespace yazi::web;

namespace yazi
{
    namespace app
    {
        class Form : public Controller
        {
        public:
            void index(const Request & req, Response & resp);
            void reply(const Request & req, Response & resp);
        };
    }
}