#include "GetCommand.hpp"

#include "../../../utils.hpp"

using namespace std;

GetCommand::ParseResult GetCommand::parse(const vector<string>& raw) {
    if(raw.size() < 2) {
        return ParseResult::fail(CommandError::WRONG_ARG_COUNT);
    }

    unique_ptr<GetArgs> args = make_unique<GetArgs>();
    args->key = raw[1];

    return ParseResult::ok(move(args));
}

vector<Event> GetCommand::execute_impl(Session& session, Storage& storage, const ArgsBase& args) {
    const GetArgs& get_args = static_cast<const GetArgs&>(args);

    Entry* existing_entry = storage.get(get_args.key);

    if(existing_entry == nullptr) {
        session.send(RespValue::null_string());
        return {};
    }

    const auto* str_ptr = std::get_if<std::string>(&existing_entry->val);
    
    if (!str_ptr) {
        session.send(RespValue::error(CommandError::WRONGTYPE));
        return {};
    }

    session.send(RespValue::bulk_string(*str_ptr));
    return {};
}


