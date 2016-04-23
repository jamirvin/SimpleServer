#pragma once
#include <string>

class HttpMessage {

    public:
    std::string file;
    std::string method;
    std::string type;

    HttpMessage(const char* input);

};
