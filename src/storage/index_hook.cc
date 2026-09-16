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

#include "storage/index_hook.h"

#include <algorithm>
#include <mutex>
#include <shared_mutex>

#include "storage/storage.h"

namespace engine {

void Storage::RegisterIndexHook(IndexHook *hook) {
  if (hook == nullptr) return;

  std::unique_lock lock(index_hooks_mu_);
  if (std::find(index_hooks_.begin(), index_hooks_.end(), hook) == index_hooks_.end()) {
    index_hooks_.push_back(hook);
  }
}

void Storage::UnregisterIndexHook(IndexHook *hook) {
  std::unique_lock lock(index_hooks_mu_);
  index_hooks_.erase(std::remove(index_hooks_.begin(), index_hooks_.end(), hook), index_hooks_.end());
}

size_t Storage::IndexHookCount() {
  std::shared_lock lock(index_hooks_mu_);
  return index_hooks_.size();
}

void Storage::notifyIndexHooks(engine::Context &ctx, const rocksdb::WriteBatch &updates) {
  std::shared_lock lock(index_hooks_mu_);
  for (auto *hook : index_hooks_) {
    hook->OnWriteCommitted(ctx, updates);
  }
}

}  // namespace engine
