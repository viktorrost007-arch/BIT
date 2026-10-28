#include "OllamaTestHeaders.h"



string runTool(const string &name, const json &args)
{
    if (name == "run_shell"){
        return executeCommand(args["command"]);
    }
    // if (name == "readNotices"){
    //     return readNotices()
    // }
    // if (name == "makeNotice"){
    //     return makeNotice()
    // }
    else {
        return "unknown tool: " + name;
    }
}



string executeCommand(const string &cmd)
{
    FILE *p = popen((cmd + " 2>&1").c_str(), "r");
    if (!p)
        return "[popen failed]";

    string out;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), p)) > 0)
    {
        out.append(buf, n);
    }
    pclose(p);

    if (out.empty())
        out = "[no output]";
    if (out.size() > 4000)
        out = out.substr(0, 4000) + "\n...(truncated)";
    return out;
}

// string readNotices(){

// }

// string makeNotice(){

// }