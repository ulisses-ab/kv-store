#pragma once

#include "../Command.hpp"
#include <utility>

class LrangeCommand : public Command {
private:
    struct LrangeArgs : public ArgsBase {
        std::string key;
        int start, stop;
    };

    ParseResult parse(const std::vector<std::string>& raw) override;
    std::vector<Event> execute_impl(Session& session, Storage& storage, const ArgsBase& args) override;

    std::pair<int, int> compute_effective_indices(int list_size, int start, int stop);
};