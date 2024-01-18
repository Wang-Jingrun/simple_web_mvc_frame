#include <app/index.h>
using namespace yazi::app;

CONTROLLER(Index);

ACTION(Index, index);
void Index::index(const Request & req, Response & resp)
{
    resp.html("<h1 style=\"text-align: center; font-size: 64px;\">yazi-web</h1><p style=\"text-align: center; font-size: 36px;\">a lightweight c++ web mvc framework</p>");
}

ACTION(Index, show);
void Index::show(const Request & req, Response & resp)
{
    resp.html("index-show");
}