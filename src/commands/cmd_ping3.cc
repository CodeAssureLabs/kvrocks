#include "commander.h"
#include "server/redis_reply.h"
#include "server/server.h"

namespace redis {

class CommandPing3 : public Commander {
 public:
  Status Execute([[maybe_unused]] engine::Context &ctx, [[maybe_unused]] Server *srv, [[maybe_unused]] Connection *conn, std::string *output) override {
    *output = redis::SimpleString("PONG3");
    return Status::OK();
  }
};

REDIS_REGISTER_COMMANDS(Ping3, MakeCmdAttr<CommandPing3>("ping3", 1, "read-only", NO_KEY));

}  // namespace redis
