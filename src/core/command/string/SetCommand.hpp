#pragma once

#include "../Command.hpp"

class SetCommand : public Command {
private:
    struct SetArgs : public ArgsBase {
        std::string key;
        std::string value;
        std::optional<uint64_t> duration_ms;
        bool keep_ttl;
        bool nx;
        bool xx;
    };

    ParseResult parse(const std::vector<std::string>& raw) override;
    std::vector<Event> execute_impl(Session& session, Storage& storage, const ArgsBase& args) override;
};