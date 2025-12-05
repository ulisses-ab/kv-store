#pragma once

#include <string>
#include <optional>
#include <variant>
#include <unordered_map>
#include <functional>
#include <deque>

using DbValue = std::variant<std::string, std::deque<std::string>>;

struct Entry { 
    DbValue val;
    std::optional<uint64_t> expiration;
};

class Storage {
public:
    Entry* get(const std::string& key);
    const Entry* get(const std::string& key) const;

    Entry* set(std::string key, DbValue val, std::optional<uint64_t>  duration_ms);
    bool expire(const std::string& key, std::optional<uint64_t> duration_ms);
    std::optional<DbValue> del(const std::string& key);
    
    void clear();
private:
    std::unordered_map<std::string, Entry>::iterator find_valid(const std::string& key);
    std::unordered_map<std::string, Entry>::const_iterator find_valid(const std::string& key) const;

    std::optional<uint64_t> compute_expiration(std::optional<uint64_t> duration_ms) const;
    bool is_expired(const Entry& entry) const;

    std::unordered_map<std::string, Entry> map_;
};