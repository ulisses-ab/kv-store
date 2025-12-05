#include "SetCommand.hpp"

#include "../parsing_helpers.hpp"
#include "../../../utils.hpp"

using namespace std;
using namespace parsing_helpers;

SetCommand::ParseResult SetCommand::parse(const vector<string>& raw) {
    if(raw.size() < 3) {
        return ParseResult::fail(CommandError::WRONG_ARG_COUNT);
    }

    unique_ptr<SetArgs> args = make_unique<SetArgs>();
    args->key = raw[1];
    args->value = raw[2];

    vector<FlagInfo> ex_flags = {{"EX", 1}, {"PX", 1}, {"KEEPTTL", 0}};
    vector<string> ex_args;

    vector<FlagInfo> nx_xx = {{"NX", 0}, {"XX", 0}};

    for(size_t i = 3; i < raw.size(); i++) {
        try {
            if(parsing_helpers::parse_mutually_exclusive_flags(raw, i, ex_flags, &ex_args)) continue;
            if(parsing_helpers::parse_mutually_exclusive_flags(raw, i, nx_xx)) continue;
        }
        catch(const std::runtime_error& e) {
            return ParseResult::fail(std::string(CommandError::SYNTAX) + ": " + e.what());
        }

        return ParseResult::fail(CommandError::SYNTAX);
    }

    try {
        if(ex_flags[0].is_set) {
            args->duration_ms = 1000 * stoull(ex_args[0]);
        }
        else if(ex_flags[1].is_set) {
            args->duration_ms = stoull(ex_args[0]);
        }
        else if(ex_flags[2].is_set) {
            args->duration_ms = std::nullopt;
            args->keep_ttl = true; 
        }
    }
    catch(const std::invalid_argument&) {
        return ParseResult::fail(CommandError::INVALID_INT);
    }
    catch(const std::out_of_range&) {
        return ParseResult::fail(CommandError::OUT_OF_RANGE);
    }

    args->nx = nx_xx[0].is_set;
    args->xx = nx_xx[1].is_set;

    return ParseResult::ok(move(args));
}

vector<Event> SetCommand::execute_impl(Session& session, Storage& storage, const ArgsBase& args) {
    const SetArgs& set_args = static_cast<const SetArgs&>(args);

    Entry* existing_entry = storage.get(set_args.key);

    if(set_args.nx && existing_entry != nullptr) {
        session.send(RespValue::null_string());
        return {};
    }

    if(set_args.xx && existing_entry == nullptr) {
        session.send(RespValue::null_string());
        return {};
    }

    auto duration = set_args.duration_ms;

    if(set_args.keep_ttl && existing_entry != nullptr && existing_entry->expiration.has_value()) {
        duration = *existing_entry->expiration - current_time_ms();
    }

    storage.set(set_args.key, set_args.value, duration);

    session.send(RespValue::simple_string("OK"));
    return {};
}


