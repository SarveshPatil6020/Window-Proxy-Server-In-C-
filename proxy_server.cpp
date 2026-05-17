#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <semaphore>
#include <cstring>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include "proxy_parse.hpp"
#include "lru_cache.hpp"

#define BUFFER_SIZE 8192
#define MAX_CLIENTS 100

LRUCache cache(50);

std::counting_semaphore<MAX_CLIENTS> semaphore(MAX_CLIENTS);

SOCKET connectRemoteServer(const std::string& host,
                           int port)
{
    std::cout << "[+] Resolving hostname..."
              << std::endl;

    struct addrinfo hints{}, *result, *ptr;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string portStr = std::to_string(port);

    int res = getaddrinfo(
        host.c_str(),
        portStr.c_str(),
        &hints,
        &result
    );

    if (res != 0)
    {
        std::cout << "[-] getaddrinfo failed: "
                  << WSAGetLastError()
                  << std::endl;

        return INVALID_SOCKET;
    }

    SOCKET remoteSocket = INVALID_SOCKET;

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        remoteSocket = socket(
            ptr->ai_family,
            ptr->ai_socktype,
            ptr->ai_protocol
        );

        if (remoteSocket == INVALID_SOCKET)
        {
            continue;
        }

        DWORD timeout = 10000;

        setsockopt(
            remoteSocket,
            SOL_SOCKET,
            SO_RCVTIMEO,
            (const char*)&timeout,
            sizeof(timeout)
        );

        setsockopt(
            remoteSocket,
            SOL_SOCKET,
            SO_SNDTIMEO,
            (const char*)&timeout,
            sizeof(timeout)
        );

        char ipstr[INET_ADDRSTRLEN];

        sockaddr_in* ipv4 =
            (sockaddr_in*)ptr->ai_addr;

        inet_ntop(
            AF_INET,
            &(ipv4->sin_addr),
            ipstr,
            sizeof(ipstr)
        );

        std::cout << "[+] Trying IP: "
                  << ipstr
                  << std::endl;

        if (connect(
                remoteSocket,
                ptr->ai_addr,
                (int)ptr->ai_addrlen
            ) == SOCKET_ERROR)
        {
            std::cout << "[-] Connect failed. Error: "
                      << WSAGetLastError()
                      << std::endl;

            closesocket(remoteSocket);

            remoteSocket = INVALID_SOCKET;

            continue;
        }

        std::cout << "[+] Connected Successfully!"
                  << std::endl;

        break;
    }

    freeaddrinfo(result);

    return remoteSocket;
}

void handleClient(SOCKET clientSocket)
{
    semaphore.acquire();

    std::cout << "\n===================================="
              << std::endl;

    std::cout << "[+] Client Connected"
              << std::endl;

    char buffer[BUFFER_SIZE];

    memset(buffer, 0, BUFFER_SIZE);

    int received =
        recv(clientSocket,
             buffer,
             BUFFER_SIZE - 1,
             0);

    if (received <= 0)
    {
        std::cout << "[-] Failed to receive request"
                  << std::endl;

        closesocket(clientSocket);

        semaphore.release();

        return;
    }

    buffer[received] = '\0';

    std::string requestStr(buffer);

    std::cout << "\n[REQUEST RECEIVED]\n"
              << requestStr
              << std::endl;

    if (cache.exists(requestStr))
    {
        std::cout << "[CACHE HIT]"
                  << std::endl;

        std::string cachedResponse =
            cache.get(requestStr);

        send(clientSocket,
             cachedResponse.c_str(),
             (int)cachedResponse.size(),
             0);

        closesocket(clientSocket);

        semaphore.release();

        return;
    }

    std::cout << "[CACHE MISS]"
              << std::endl;

    ParsedRequest request;

    if (!request.parse(requestStr))
    {
        std::cout << "[-] Request parsing failed"
                  << std::endl;

        closesocket(clientSocket);

        semaphore.release();

        return;
    }

    std::cout << "\n[PARSED REQUEST]"
              << std::endl;

    std::cout << "Method : "
              << request.method
              << std::endl;

    std::cout << "Host   : "
              << request.host
              << std::endl;

    std::cout << "Path   : "
              << request.path
              << std::endl;

    std::cout << "Version: "
              << request.version
              << std::endl;

    std::cout << "Port   : "
              << request.port
              << std::endl;

    SOCKET remoteSocket =
        connectRemoteServer(
            request.host,
            std::stoi(request.port)
        );

    if (remoteSocket == INVALID_SOCKET)
    {
        std::cout << "[-] Could not connect remote server"
                  << std::endl;

        closesocket(clientSocket);

        semaphore.release();

        return;
    }

    std::string forwardRequest;

    forwardRequest +=
        request.method + " ";

    forwardRequest +=
        request.path + " ";

    forwardRequest +=
        request.version + "\r\n";

    forwardRequest +=
        "Host: " + request.host + "\r\n";

    forwardRequest +=
        "Connection: close\r\n";

    forwardRequest +=
        "User-Agent: Mozilla/5.0\r\n";

    forwardRequest +=
        "\r\n";

    std::cout << "\n[FORWARDING REQUEST]\n"
              << forwardRequest
              << std::endl;

    int sent =
        send(remoteSocket,
             forwardRequest.c_str(),
             (int)forwardRequest.size(),
             0);

    if (sent == SOCKET_ERROR)
    {
        std::cout << "[-] Failed sending request"
                  << std::endl;

        closesocket(remoteSocket);
        closesocket(clientSocket);

        semaphore.release();

        return;
    }

    std::string fullResponse;

    while (true)
    {
        memset(buffer, 0, BUFFER_SIZE);

        int bytes =
            recv(remoteSocket,
                 buffer,
                 BUFFER_SIZE,
                 0);

        if (bytes <= 0)
            break;

        send(clientSocket,
             buffer,
             bytes,
             0);

        fullResponse.append(buffer, bytes);

        std::cout << "[+] Forwarded "
                  << bytes
                  << " bytes"
                  << std::endl;
    }

    std::cout << "[+] Total response size: "
              << fullResponse.size()
              << " bytes"
              << std::endl;

    if (!fullResponse.empty())
    {
        cache.put(requestStr, fullResponse);

        std::cout << "[+] Cached response"
                  << std::endl;
    }

    shutdown(remoteSocket, SD_BOTH);

    closesocket(remoteSocket);

    shutdown(clientSocket, SD_BOTH);

    closesocket(clientSocket);

    std::cout << "[+] Client disconnected"
              << std::endl;

    std::cout << "====================================\n"
              << std::endl;

    semaphore.release();
}

int main(int argc, char* argv[])
{
    int port = 8080;

    if (argc == 2)
    {
        port = atoi(argv[1]);
    }

    std::cout << "===================================="
              << std::endl;

    std::cout << "Multithreaded Proxy Server Starting"
              << std::endl;

    std::cout << "Port: "
              << port
              << std::endl;

    std::cout << "===================================="
              << std::endl;

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2),
                   &wsaData) != 0)
    {
        std::cout << "[-] WSAStartup failed"
                  << std::endl;

        return 1;
    }

    std::cout << "[+] WinSock initialized"
              << std::endl;

    SOCKET serverSocket =
        socket(AF_INET,
               SOCK_STREAM,
               IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET)
    {
        std::cout << "[-] Socket creation failed"
                  << std::endl;

        WSACleanup();

        return 1;
    }

    BOOL opt = TRUE;

    setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        (char*)&opt,
        sizeof(opt)
    );

    std::cout << "[+] Server socket created"
              << std::endl;

    sockaddr_in serverAddr{};

    serverAddr.sin_family = AF_INET;

    serverAddr.sin_port = htons(port);

    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket,
             (sockaddr*)&serverAddr,
             sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cout << "[-] Bind failed"
                  << std::endl;

        closesocket(serverSocket);

        WSACleanup();

        return 1;
    }

    std::cout << "[+] Bind successful"
              << std::endl;

    if (listen(serverSocket,
               SOMAXCONN) == SOCKET_ERROR)
    {
        std::cout << "[-] Listen failed"
                  << std::endl;

        closesocket(serverSocket);

        WSACleanup();

        return 1;
    }

    std::cout << "[+] Proxy Server Running..."
              << std::endl;

    while (true)
    {
        std::cout << "\n[WAITING FOR CLIENT CONNECTION...]"
                  << std::endl;

        SOCKET clientSocket =
            accept(serverSocket,
                   nullptr,
                   nullptr);

        if (clientSocket == INVALID_SOCKET)
        {
            std::cout << "[-] Accept failed"
                      << std::endl;

            continue;
        }

        std::thread clientThread(
            handleClient,
            clientSocket
        );

        clientThread.detach();
    }

    closesocket(serverSocket);

    WSACleanup();

    return 0;
}