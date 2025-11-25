#pragma once

#include <string>
#include <optional>
#include <variant>
#include <unordered_map>
#include <functional>

using DbValue = std::variant<std::string, int>;

class Storage {
public:
    std::optional<std::reference_wrapper<DbValue>> get(const std::string& key);
    std::optional<std::reference_wrapper<const DbValue>> get(const std::string& key) const;

    void set(std::string key, DbValue val, std::optional<uint64_t>  duration_ms);
    bool expire(const std::string& key, std::optional<uint64_t> duration_ms);
    std::optional<DbValue> del(const std::string& key);
    
    void clear();
private:
    struct Entry { 
        DbValue val;
        std::optional<uint64_t> expiration;
    };

    std::unordered_map<std::string, Entry>::iterator find_valid(const std::string& key);
    std::unordered_map<std::string, Entry>::const_iterator find_valid(const std::string& key) const;

    std::optional<uint64_t> compute_expiration(std::optional<uint64_t> duration_ms) const;
    bool is_expired(const Entry& entry) const;

    std::unordered_map<std::string, Entry> map_;
};