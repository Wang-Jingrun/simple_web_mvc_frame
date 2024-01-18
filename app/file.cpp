#include <app/file.h>
using namespace yazi::app;

CONTROLLER(File);

ACTION(File, index);
void File::index(const Request & req, Response & resp)
{
    resp.render("file/index.html");
}

ACTION(File, upload);
void File::upload(const Request & req, Response & resp)
{
    FileUpload file = req.file("file");

    const string & filename = "./upload/" + file.filename();
    file.save(filename);
    
    Json json;
    json["code"] = 0;
    json["data"] = "上传成功";
    return resp.json(json.str());
}