# Multithreaded Proxy Web Server in C++ (Windows)

A high-performance multithreaded proxy web server built using modern C++ for Windows.

This project converts a Linux-based C proxy server into a Windows-compatible C++ implementation using:

- WinSock2
- Multithreading (`std::thread`)
- Synchronization (`std::mutex`)
- Counting Semaphores (`std::counting_semaphore`)
- LRU Cache
- HTTP Request Parsing
- Socket Programming

---

# Features

- Multithreaded proxy server
- Concurrent client handling
- Thread-safe LRU cache
- HTTP GET request forwarding
- DNS resolution
- Windows socket programming
- Modern C++ implementation
- Cache hit/miss logging
- Proxy request parsing
- Scalable architecture

---

# Technologies Used

- C++
- WinSock2
- CMake
- STL
- Networking
- Operating Systems Concepts
- Synchronization
- Socket Programming

---

# OS Concepts Used

- Threading
- Mutex Locks
- Counting Semaphore
- Synchronization
- Socket Communication
- LRU Cache Management
- Concurrent Request Handling

---

# Project Structure

```text
MultithreadedProxyServer/
│
├── proxy_server.cpp
├── proxy_parse.cpp
├── proxy_parse.hpp
├── lru_cache.cpp
├── lru_cache.hpp
├── CMakeLists.txt
└── README.md
```

---

# Requirements

## Software Required

- Windows 10 / Windows 11
- Visual Studio 2022
OR
- MinGW-w64
OR
- CLion
- CMake 3.15+
- C++20 Compiler

---

# Important Configuration

This project uses:

```cpp
std::counting_semaphore
```

which requires:

```text
C++20
```

---

# Visual Studio Configuration

If using Visual Studio:

Open:

```text
proxy_server.vcxproj
```

Find:

```xml
<LanguageStandard>
```

Replace with:

```xml
<LanguageStandard>stdcpp20</LanguageStandard>
```

---

# CMake Configuration

Use:

```bash
cmake .. -DCMAKE_CXX_STANDARD=20
```

---

# Build Instructions

## Step 1: Clone Repository

```bash
git clone <your-github-repo-link>
```

---

## Step 2: Open Project Directory

```bash
cd MultithreadedProxyServer
```

---

## Step 3: Create Build Folder

```bash
mkdir build
```

---

## Step 4: Move into Build Folder

```bash
cd build
```

---

## Step 5: Generate Build Files

```bash
cmake .. -DCMAKE_CXX_STANDARD=20
```

---

## Step 6: Build Project

```bash
cmake --build .
```

---

# Running the Proxy Server

After successful build:

```bash
Debug\proxy_server.exe 8080
```

OR

```bash
Release\proxy_server.exe 8080
```

You should see:

```text
Proxy Server Running...
```

---

# Testing the Proxy Server

## Method 1: Using Another Terminal

Open a NEW terminal while server is running.

Run:

```bash
curl.exe --proxy http://127.0.0.1:8080 http://example.com -v
```

OR

```bash
curl.exe --proxy http://127.0.0.1:8080 http://httpforever.com -v
```

---

# Example Workflow

## Terminal 1

Run server:

```bash
Debug\proxy_server.exe 8080
```

---

## Terminal 2

Send request:

```bash
curl.exe --proxy http://127.0.0.1:8080 http://example.com -v
```

---

# Browser Proxy Setup

## Windows Proxy Settings

Go to:

```text
Settings → Network & Internet → Proxy
```

Enable:

```text
Use a proxy server
```

Set:

```text
Address : 127.0.0.1
Port    : 8080
```

---

# Supported Features

| Feature | Status |
|---|---|
| HTTP Proxy | ✅ |
| GET Requests | ✅ |
| Multithreading | ✅ |
| Semaphore Synchronization | ✅ |
| LRU Cache | ✅ |
| DNS Resolution | ✅ |
| Windows Support | ✅ |
| HTTPS CONNECT | ❌ |
| SSL/TLS Tunneling | ❌ |

---

# Important Notes

## 1. HTTPS Websites

This proxy currently supports:

```text
HTTP only
```

Modern websites mostly use HTTPS.

Therefore:

- Google
- YouTube
- Wikipedia
- Instagram

may not work until CONNECT tunneling is implemented.

---

## 2. Recommended Test Websites

Use:

```text
http://example.com
http://httpforever.com
```

---

## 3. Windows Firewall

If remote connection fails with:

```text
Error Code: 10060
```

temporarily disable:

- Windows Defender Firewall
- Antivirus
- VPN

---

## 4. IPv4 + IPv6 Support

Use:

```cpp
hints.ai_family = AF_UNSPEC;
```

instead of:

```cpp
AF_INET
```

for better compatibility.

---

# Cache System

This project uses:

```text
LRU (Least Recently Used) Cache
```

Features:

- Thread-safe
- Automatic eviction
- Faster repeated requests
- Reduces server load

---

# Future Improvements

- HTTPS CONNECT tunneling
- SSL/TLS support
- POST request handling
- Full browser compatibility
- Async sockets
- Better cache persistence
- Logging system
- Authentication support

---

# Learning Outcomes

This project demonstrates understanding of:

- Operating Systems
- Computer Networks
- Socket Programming
- Concurrent Programming
- Synchronization
- Proxy Servers
- C++ System Programming

---

# Author

Sarvesh Patil

B.Tech Computer Engineering  
Vishwakarma Institute of Information Technology, Pune

---

# License

This project is for educational and learning purposes.
