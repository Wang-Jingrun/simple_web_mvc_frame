#include <app/post.h>
using namespace yazi::app;

CONTROLLER(Post);

ACTION(Post, index);
void Post::index(const Request & req, Response & resp)
{
    resp.render("json/index.html");
}

ACTION(Post, json);
void Post::json(const Request & req, Response & resp)
{
    string name = req.post("name");
    int age = req.post("age");

    Json json;
    json["name"] = name;
    json["age"] = age;
    resp.json(json.str());
}
