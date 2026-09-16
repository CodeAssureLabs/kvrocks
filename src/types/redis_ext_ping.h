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

#pragma once

#include <optional>
#include <string>

namespace redis {

// ExtPing computes the payload of a PING reply.
//
// It lives in the type (domain) layer and therefore knows nothing about the
// server or the connection: the command layer decides how to frame the payload
// (simple string vs. bulk string) and owns any server state.
class ExtPing {
 public:
  ExtPing() = default;
  explicit ExtPing(std::string message) : message_(std::move(message)) {}

  // True when the client supplied an explicit message to echo back.
  bool HasMessage() const { return message_.has_value(); }

  // "PONG" when no message was supplied, otherwise the message itself.
  std::string Reply() const;

 private:
  std::optional<std::string> message_;
};

}  // namespace redis
