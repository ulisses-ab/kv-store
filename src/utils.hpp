#pragma once

#include <cstdint>
#include <chrono>

inline uint64_t current_time_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}

inline std::string to_upper(std::string str) {
    for(auto& c : str) {
        c = toupper(c);
    }

    return str;
}

inline std::string to_lower(std::string str) {
    for(auto& c : str) {
        c = tolower(c);
    }

    return str;
}