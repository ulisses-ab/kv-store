#include "LpushCommand.hpp"

using namespace std;

LpushCommand::ParseResult LpushCommand::parse(const vector<string>& raw) {
    if(raw.size() < 3) {
        return ParseResult::fail(CommandError::WRONG_ARG_COUNT);
    }
    
    unique_ptr<LpushArgs> args = make_unique<LpushArgs>();
    args->key = raw[1];

    for(size_t i = 2; i < raw.size(); i++) {
        args->values.push_back(raw[i]);
    }

    return ParseResult::ok(move(args));
}

vector<Event> LpushCommand::execute_impl(Session& session, Storage& storage, const ArgsBase& args) {
    const LpushArgs& lpush_args = static_cast<const LpushArgs&>(args);

    Entry* existing_entry = storage.get(lpush_args.key);

    if(existing_entry != nullptr && !holds_alternative<StringList>(existing_entry->val)) {
        session.send(RespValue::error(CommandError::WRONGTYPE));
        return {};
    }

    if(existing_entry == nullptr) {
        existing_entry = storage.set(lpush_args.key, StringList(), std::nullopt);
    }   

    StringList& str_list = get<StringList>(existing_entry->val);

    for(const string& val : lpush_args.values) {
        str_list.push_front(val);
    }

    session.send(RespValue::integer(str_list.size()));
    return {};
}


