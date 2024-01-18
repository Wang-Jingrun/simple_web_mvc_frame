#pragma once

#include <web/request.h>
#include <web/response.h>

#include <reflect/class_register.h>
using namespace yazi::reflect;

namespace yazi
{
    namespace web
    {
        class Controller : public Object
        {
        public:
            Controller() {}
            virtual ~Controller() {}
        };

        #define CONTROLLER(classname) \
                REGISTER_CLASS(classname)

        #define ACTION(classname, method) \
                REGISTER_CLASS_METHOD(classname, method, void, const Request &, Response &)

    }
}
