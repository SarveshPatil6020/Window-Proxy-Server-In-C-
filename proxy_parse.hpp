#ifndef PROXY_PARSE_HPP
#define PROXY_PARSE_HPP

#include <string>
#include <map>

class ParsedRequest
{
public:
    std::string method;
    std::string host;
    std::string path;
    std::string version;
    std::string port;

    std::map<std::string, std::string> headers;

    bool parse(const std::string& request);

    std::string buildRequest() const;
};

#endif