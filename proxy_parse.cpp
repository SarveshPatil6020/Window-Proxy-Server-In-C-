#include "proxy_parse.hpp"

#include <sstream>
#include <algorithm>

bool ParsedRequest::parse(const std::string& request)
{
    std::istringstream stream(request);

    std::string line;

    if (!std::getline(stream, line))
        return false;

    if (!line.empty() && line.back() == '\r')
        line.pop_back();

    std::istringstream firstLine(line);

    std::string url;

    firstLine >> method >> url >> version;

    if (method.empty() || url.empty() || version.empty())
        return false;

    if (url.find("http://") == 0)
    {
        url = url.substr(7);
    }

    size_t slashPos = url.find('/');

    if (slashPos == std::string::npos)
    {
        host = url;
        path = "/";
    }
    else
    {
        host = url.substr(0, slashPos);
        path = url.substr(slashPos);
    }

    size_t colonPos = host.find(':');

    if (colonPos != std::string::npos)
    {
        port = host.substr(colonPos + 1);
        host = host.substr(0, colonPos);
    }
    else
    {
        port = "80";
    }

    while (std::getline(stream, line))
    {
        if (line == "\r" || line.empty())
            break;

        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        size_t pos = line.find(':');

        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            value.erase(0, value.find_first_not_of(' '));

            headers[key] = value;
        }
    }

    return true;
}

std::string ParsedRequest::buildRequest() const
{
    std::ostringstream req;

    req << method << " " << path << " " << version << "\r\n";
    req << "Host: " << host << "\r\n";
    req << "Connection: close\r\n";

    for (const auto& header : headers)
    {
        if (header.first == "Host" || header.first == "Connection")
            continue;

        req << header.first << ": " << header.second << "\r\n";
    }

    req << "\r\n";

    return req.str();
}