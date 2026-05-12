#pragma once

#include "../Command.hpp"

class LlenCommand : public Command {
private:
    struct LlenArgs : public ArgsBase {
        std::string key;
    };

    ParseResult parse(const std::vector<std::string>& raw) override;
    std::vector<Event> execute_impl(Session& session, Storage& storage, const ArgsBase& args) override;
};