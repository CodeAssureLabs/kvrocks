#include "types/redis_ext_ping.h"

namespace redis {

std::string ExtPing::Reply() const { return srv_ ? "PONG" : ""; }

}  // namespace redis
