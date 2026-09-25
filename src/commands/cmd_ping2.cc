/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "commander.h"
#include "server/redis_reply.h"
#include "server/server.h"

namespace redis {

class CommandPing2 : public Commander {
 public:
  Status Execute([[maybe_unused]] engine::Context &ctx, [[maybe_unused]] Server *srv, [[maybe_unused]] Connection *conn,
                 std::string *output) override {
    *output = redis::SimpleString("PONG2");
    return Status::OK();
  }
};

REDIS_REGISTER_COMMANDS(Ping2, MakeCmdAttr<CommandPing2>("ping2", 1, "read-only", NO_KEY));

}  // namespace redis
