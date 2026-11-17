#include "server_headers.h"

string executeCommand(const string& cmd) {
    FILE* p = popen((cmd + " 2>&1").c_str(), "r");
    if (!p) return "[popen failed]";
    string out; char buf[4096];
    while (fgets(buf, sizeof(buf), p)) out += buf;
    pclose(p);
    return out.empty() ? "[no output]" : out;
}

string runTool(const string& name, const json& args) {
    if (name == "run_shell_on_laptop") {
        return call_laptop(args["command"]);
    }
    if (name == "run_shell") {
        return executeCommand(args["command"]);
    }
    return "unknown tool: " + name;
}