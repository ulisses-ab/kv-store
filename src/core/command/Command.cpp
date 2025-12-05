#include "Command.hpp"

using namespace std;

vector<Event> Command::execute(Session& session, Storage& storage, const vector<string>& raw) {
    auto parse_results = parse(raw);

    if(parse_results.fail()) {
        session.send(RespValue::error(*parse_results.error));
        return {};
    }

    return execute_impl(session, storage, *parse_results.args);
}