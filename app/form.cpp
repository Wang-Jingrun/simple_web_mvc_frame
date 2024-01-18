#include <app/form.h>
using namespace yazi::app;

CONTROLLER(Form);

ACTION(Form, index);
void Form::index(const Request & req, Response & resp)
{
    resp.render("form/index.html");
}

ACTION(Form, reply);
void Form::reply(const Request & req, Response & resp)
{
    auto name = req.post("name");
    auto age = req.post("age");

    Json json;
    json["name"] = name;
    json["age"] = age;
    resp.json(json.str());
}
