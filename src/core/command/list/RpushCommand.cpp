#include "RpushCommand.hpp"

using namespace std;

RpushCommand::ParseResult RpushCommand::parse(const vector<string>& raw) {
    if(raw.size() < 3) {
        return ParseResult::fail(CommandError::WRONG_ARG_COUNT);
    }
    
    unique_ptr<RpushArgs> args = make_unique<RpushArgs>();
    args->key = raw[1];

    for(size_t i = 2; i < raw.size(); i++) {
        args->values.push_back(raw[i]);
    }

    return ParseResult::ok(move(args));
}

vector<Event> RpushCommand::execute_impl(Session& session, Storage& storage, const ArgsBase& args) {
    const RpushArgs& rpush_args = static_cast<const RpushArgs&>(args);

    Entry* existing_entry = storage.get(rpush_args.key);

    if(existing_entry != nullptr && !holds_alternative<StringList>(existing_entry->val)) {
        session.send(RespValue::error(CommandError::WRONGTYPE));
        return {};
    }

    if(existing_entry == nullptr) {
        existing_entry = storage.set(rpush_args.key, StringList(), std::nullopt);
    }   

    StringList& str_list = get<StringList>(existing_entry->val);

    for(const string& val : rpush_args.values) {
        str_list.push_back(val);
    }

    session.send(RespValue::integer(str_list.size()));
    return {};
}


