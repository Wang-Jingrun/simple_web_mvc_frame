#include <app/user.h>
using namespace yazi::app;

CONTROLLER(User);

ACTION(User, profile);
void User::profile(const Request & req, Response & resp)
{
    Json json;
    json["uid"] = 1;
    json["name"] = "kitty";
    json["age"] = 18;
    json["gender"] = "female";
    resp.json(json.str());
}
