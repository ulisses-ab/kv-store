#include "LlenCommand.hpp"

using namespace std;

LlenCommand::ParseResult LlenCommand::parse(const vector<string>& raw) {
    if(raw.size() != 2) {
        return ParseResult::fail(CommandError::WRONG_ARG_COUNT);
    }
    
    unique_ptr<LlenArgs> args = make_unique<LlenArgs>();
    args->key = raw[1];

    return ParseResult::ok(move(args));
}

vector<Event> LlenCommand::execute_impl(Session& session, Storage& storage, const ArgsBase& args) {
    const LlenArgs& llen_args = static_cast<const LlenArgs&>(args);

    Entry* existing_entry = storage.get(llen_args.key);

    if(existing_entry != nullptr && !holds_alternative<StringList>(existing_entry->val)) {
        session.send(RespValue::error(CommandError::WRONGTYPE));
        return {};
    }

    if(existing_entry == nullptr) {
        session.send(RespValue::integer(0));
        return {};
    }   

    StringList& str_list = get<StringList>(existing_entry->val);

    session.send(RespValue::integer(str_list.size()));
    return {};
}


