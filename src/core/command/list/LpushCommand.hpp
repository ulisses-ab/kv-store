#pragma once

#include "../Command.hpp"

class LpushCommand : public Command {
private:
    struct LpushArgs : public ArgsBase {
        std::string key;
        std::vector<std::string> values;
    };

    ParseResult parse(const std::vector<std::string>& raw) override;
    std::vector<Event> execute_impl(Session& session, Storage& storage, const ArgsBase& args) override;
};