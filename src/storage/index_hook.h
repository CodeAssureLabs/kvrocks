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

#include <rocksdb/write_batch.h>

namespace engine {

struct Context;

/// IndexHook is the storage-side extension point for secondary index maintainers,
/// such as the search indexer.
///
/// The storage engine only knows about this abstract interface. Concrete hooks live
/// in upper layers (e.g. src/search) and register themselves through
/// Storage::RegisterIndexHook, so the storage engine never depends on them.
class IndexHook {
 public:
  virtual ~IndexHook() = default;

  /// Called after `updates` has been successfully committed to the underlying DB
  /// via Storage::Write (including the flush of a MULTI/EXEC transaction batch).
  ///
  /// Batches applied by replication (Storage::ReplicaApplyWriteBatch) are not
  /// reported: they already carry the index data produced on the primary.
  ///
  /// Implementations may be invoked concurrently from multiple worker threads and
  /// must not call back into Storage::RegisterIndexHook / UnregisterIndexHook.
  virtual void OnWriteCommitted(Context &ctx, const rocksdb::WriteBatch &updates) = 0;
};

}  // namespace engine
