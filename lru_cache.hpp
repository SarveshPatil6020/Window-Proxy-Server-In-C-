#ifndef LRU_CACHE_HPP
#define LRU_CACHE_HPP

#include <string>
#include <unordered_map>
#include <list>
#include <mutex>

class LRUCache
{
private:
    struct CacheEntry
    {
        std::string key;
        std::string value;
    };

    size_t maxSize;

    std::list<CacheEntry> cacheList;

    std::unordered_map<std::string,
        std::list<CacheEntry>::iterator> cacheMap;

    std::mutex cacheMutex;

public:
    explicit LRUCache(size_t size);

    bool exists(const std::string& key);

    std::string get(const std::string& key);

    void put(const std::string& key,
             const std::string& value);
};

#endif