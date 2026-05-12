#include "LrangeCommand.hpp"
#include <stdexcept>

using namespace std;

LrangeCommand::ParseResult LrangeCommand::parse(const vector<string>& raw) {
    if(raw.size() != 4) {
        return ParseResult::fail(CommandError::WRONG_ARG_COUNT);
    }
    
    unique_ptr<LrangeArgs> args = make_unique<LrangeArgs>();
    args->key = raw[1];

    try {
        args->start = stoi(raw[2]);
        args->stop = stoi(raw[3]);
    }
    catch(const std::invalid_argument&) {
        return ParseResult::fail(CommandError::INVALID_INT);
    }
    catch(const std::out_of_range&) {
        return ParseResult::fail(CommandError::OUT_OF_RANGE);
    }

    return ParseResult::ok(move(args));
}

vector<Event> LrangeCommand::execute_impl(Session& session, Storage& storage, const ArgsBase& args) {
    const LrangeArgs& lrange_args = static_cast<const LrangeArgs&>(args);

    Entry* existing_entry = storage.get(lrange_args.key);

    if(existing_entry != nullptr && !holds_alternative<StringList>(existing_entry->val)) {
        session.send(RespValue::error(CommandError::WRONGTYPE));
        return {};
    }

    if(existing_entry == nullptr) {
        session.send(RespValue::array({}));
        return {};
    }   

    StringList& str_list = get<StringList>(existing_entry->val);
    auto [start, stop] = compute_effective_indices(str_list.size(), lrange_args.start, lrange_args.stop);

    vector<RespValue> result;
    for(ssize_t i = start; i <= stop; i++) {
        result.push_back(RespValue::bulk_string(str_list[i]));
    }

    session.send(RespValue::array(move(result)));
    return {};
}

pair<int, int> LrangeCommand::compute_effective_indices(int list_size, int start, int stop) {
    if(start < 0) {
        start = list_size + start;
    }
    if(stop < 0) {
        stop = list_size + stop;
    }

    if(start < 0) start = 0;
    if(stop < 0) stop = 0;

    if(start > list_size) start = list_size;
    if(stop >= list_size) stop = list_size - 1;

    return {start, stop};
}