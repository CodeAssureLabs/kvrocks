#include "commander.h"
#include "server/redis_reply.h"
#include "server/server.h"

namespace redis {

class CommandPing2 : public Commander {
 public:
  Status Execute([[maybe_unused]] engine::Context &ctx, [[maybe_unused]] Server *srv, [[maybe_unused]] Connection *conn, std::string *output) override {
    *output = redis::SimpleString("PONG2");
    return Status::OK();
  }
};

REDIS_REGISTER_COMMANDS(Ping2, MakeCmdAttr<CommandPing2>("ping2", 1, "read-only", NO_KEY));

}  // namespace redis
