#include "CommandDispatcher.hpp"

#include "../../utils.hpp"

#include "string/SetCommand.hpp"
#include "string/GetCommand.hpp"
#include "list/RpushCommand.hpp"
#include "list/LpushCommand.hpp"
#include "list/LrangeCommand.hpp"

using namespace std;

CommandDispatcher::CommandDispatcher() {
    register_command("SET", make_unique<SetCommand>());
    register_command("GET", make_unique<GetCommand>());
    register_command("RPUSH", make_unique<RpushCommand>());
    register_command("LPUSH", make_unique<LpushCommand>());
    register_command("LRANGE", make_unique<LrangeCommand>());
}

vector<Event> CommandDispatcher::dispatch(Session& session, Storage& storage, const std::vector<std::string>& raw) {
    auto it = registered_commands_.find(to_upper(raw[0]));
    if(it == registered_commands_.end()) {
        session.send(RespValue::error("ERR unknown command '" + raw[0] + "'"));
        return {};
    }

    return it->second->execute(session, storage, raw);
}

void CommandDispatcher::register_command(string name, unique_ptr<Command> command) {
    registered_commands_.emplace(to_upper(name), move(command));
}
