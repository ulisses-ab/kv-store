#include "Storage.hpp"

#include "../utils.hpp"

using namespace std;

Entry* Storage::get(const string& key) {
    auto it = find_valid(key);
    if(it == map_.end()) {
        return nullptr;
    }
    return &it->second;
}

const Entry* Storage::get(const string& key) const {
    auto it = find_valid(key);
    if(it == map_.end()) {
        return nullptr;
    }
    return &it->second;
}

Entry* Storage::set(string key, DbValue val, optional<uint64_t> duration_ms) {
    auto it = map_.insert_or_assign(
        move(key), 
        Entry{move(val), compute_expiration(duration_ms)}
    ).first;

    return &it->second;
}

bool Storage::expire(const string& key, optional<uint64_t> duration_ms) {
    auto it = find_valid(key);
    if(it == map_.end()) {
        return false;
    }

    it->second.expiration = compute_expiration(duration_ms);
    return true;
}

optional<DbValue> Storage::del(const string& key) {
    auto it = find_valid(key);
    if(it == map_.end()) {
        return nullopt;
    }

    DbValue v = move(it->second.val);
    map_.erase(it);
    return v;
}

void Storage::clear() {
    map_.clear();
} 

std::unordered_map<std::string, Entry>::iterator Storage::find_valid(const string& key) {
    auto it = map_.find(key);
    if(it == map_.end()) return map_.end();

    if(is_expired(it->second)) {
        map_.erase(it);
        return map_.end();
    }

    return it;
}

std::unordered_map<std::string, Entry>::const_iterator Storage::find_valid(const string& key) const {
    auto it = map_.find(key);
    if(it == map_.end()) return map_.end();

    if(is_expired(it->second)) {
        return map_.end();
    }

    return it;
}

bool Storage::is_expired(const Entry& entry) const {
    return entry.expiration && current_time_ms() >= *entry.expiration;
}

optional<uint64_t> Storage::compute_expiration(optional<uint64_t> duration_ms) const {
    return duration_ms ? 
        optional<uint64_t>(current_time_ms() + *duration_ms) : 
        nullopt;
}