#pragma once
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>

namespace parsing_helpers {

inline bool parse_flag_with_args(
    const std::vector<std::string>& raw,
    size_t& i,
    const std::string& name,
    size_t num_args,
    std::vector<std::string>& out_args
) {
    if(raw[i] != name) {
        return false;
    }

    if(i + num_args >= raw.size()) {
        return false; 
    }

    for(size_t j = 1; j <= num_args; j++) {
        out_args.push_back(raw[i + j]);
    }

    i += num_args; 
    return true;
};

inline bool parse_boolean_flag(
    const std::vector<std::string>& raw,
    size_t& i,
    bool& flag,
    const std::string& name
) {
    if(raw[i] != name) {
        return false;
    }

    flag = true;
    return true;
};

struct FlagInfo {
    std::string name;
    size_t num_args;
    bool is_set = false;
};

inline bool parse_mutually_exclusive_flags(
    const std::vector<std::string>& raw,
    size_t& i,
    std::vector<FlagInfo>& flags,
    std::vector<std::string>* out_args = nullptr
) {
    if(out_args != nullptr) {
        out_args->clear();
    }

    for(auto& flag : flags) {
        if(raw[i] != flag.name) continue; 

        for(const auto& other : flags) {
            if(other.is_set) {
                throw std::runtime_error("Flags " + flag.name + " and " + other.name + " are mutually exclusive");
            }
        }

        flag.is_set = true;

        if(i + flag.num_args >= raw.size()) {
            throw std::runtime_error("Flag " + flag.name + " expects " + std::to_string(flag.num_args) + " argument(s)");
        }


        if(out_args != nullptr) {
            for(size_t j = 1; j <= flag.num_args; ++j) {
                out_args->push_back(raw[i + j]);
            }
        }

        i += flag.num_args;
        return true;
    }

    return false;
}

}