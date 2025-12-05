#pragma once

#include "../Session.hpp"
#include "../../storage/Storage.hpp"
#include "../event/EventBus.hpp"
#include <vector>
#include <memory>

class Command {
public:
    std::vector<Event> execute(Session& session, Storage& storage, const std::vector<std::string>& raw);
protected:
    struct ArgsBase { 
        virtual ~ArgsBase() = default; 
    };

    struct ParseResult {
        std::unique_ptr<ArgsBase> args;
        std::optional<std::string> error;

        static ParseResult ok(std::unique_ptr<ArgsBase> a) {
            return { std::move(a), std::nullopt };
        }

        static ParseResult fail(std::string msg) {
            return { nullptr, std::move(msg) };
        }

        bool success() { return !error.has_value(); }
        bool fail() { return error.has_value(); }
    };

    struct CommandError { 
        static constexpr const char* WRONG_ARG_COUNT = "ERR wrong number of arguments"; 
        static constexpr const char* INVALID_INT = "ERR value is not an integer"; 
        static constexpr const char* SYNTAX = "ERR syntax error"; 
        static constexpr const char* OUT_OF_RANGE = "ERR value out of range"; 
        static constexpr const char* UNKNOWN = "ERR unknown error"; 
        static constexpr const char* WRONGTYPE = "WRONGTYPE Operation against a key holding the wrong kind of value"; 
    };

    virtual ParseResult parse(const std::vector<std::string>& raw) = 0;
    virtual std::vector<Event> execute_impl(Session&, Storage&, const ArgsBase& args) = 0;
};