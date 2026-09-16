#pragma once

#include <string>

#include "server/server.h"
#include "storage/storage.h"

namespace redis {

class ExtPing {
 public:
  explicit ExtPing(Server *srv) : srv_(srv) {}
  std::string Reply() const;

 private:
  Server *srv_;
};

}  // namespace redis
