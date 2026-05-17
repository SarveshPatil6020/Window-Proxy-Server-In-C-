#include "lru_cache.hpp"

LRUCache::LRUCache(size_t size)
{
    maxSize = size;
}

bool LRUCache::exists(const std::string& key)
{
    std::lock_guard<std::mutex> lock(cacheMutex);

    return cacheMap.find(key) != cacheMap.end();
}

std::string LRUCache::get(const std::string& key)
{
    std::lock_guard<std::mutex> lock(cacheMutex);

    auto it = cacheMap.find(key);

    if (it == cacheMap.end())
        return "";

    cacheList.splice(cacheList.begin(),
                     cacheList,
                     it->second);

    return it->second->value;
}

void LRUCache::put(const std::string& key,
                   const std::string& value)
{
    std::lock_guard<std::mutex> lock(cacheMutex);

    auto it = cacheMap.find(key);

    if (it != cacheMap.end())
    {
        cacheList.erase(it->second);
        cacheMap.erase(it);
    }

    cacheList.push_front({key, value});

    cacheMap[key] = cacheList.begin();

    if (cacheList.size() > maxSize)
    {
        auto last = cacheList.end();
        --last;

        cacheMap.erase(last->key);

        cacheList.pop_back();
    }
}