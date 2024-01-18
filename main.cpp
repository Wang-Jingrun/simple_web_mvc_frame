#include <iostream>
using namespace std;

#include <web/server.h>
using namespace yazi::web;

void hello(const Request & req, Response & resp)
{
    string name = req.get("name");
    string age = req.get("age");
    string host = req.user_host();
    resp.data(Response::HTML, "hello, " + name + "," + age + "," + host);
}

void reply(const Request & req, Response & resp)
{
    Json name = req.post("name");
    Json age = req.post("age");

    if (name.is_null())
    {
        // 姓名为空
    }
    
    Json json;
    json["name"] = name;
    json["age"] = age;
    resp.data(Response::JSON, json.str());
}

int main()
{
    auto server = Singleton<Server>::instance();
    server->bind("/hello", hello);
    server->bind("/reply", reply);
    server->start();
    return 0;
}
