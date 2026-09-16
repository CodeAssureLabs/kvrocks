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

#include <gtest/gtest.h>
#include <rocksdb/merge_operator.h>
#include <rocksdb/write_batch.h>

#include "storage/batch_indexer.h"
#include "test_base.h"

class WriteBatchIndexerTest : public TestBase {
 protected:
  explicit WriteBatchIndexerTest() = default;
  ~WriteBatchIndexerTest() override = default;
};

TEST_F(WriteBatchIndexerTest, PutDelete) {
  rocksdb::WriteBatch batch;
  auto s = batch.Put("key0", "value0");
  EXPECT_TRUE(s.ok()) << s.ToString();
  for (int i = 1; i < 4; ++i) {
    std::string key = "key" + std::to_string(i);
    std::string value = "value" + std::to_string(i);
    s = batch.Put(storage_->GetCFHandle(ColumnFamilyID::PrimarySubkey), key, value);
    EXPECT_TRUE(s.ok()) << s.ToString();
  }

  ctx_->batch = std::make_unique<rocksdb::WriteBatchWithIndex>();
  WriteBatchIndexer handle1(*ctx_);
  s = batch.Iterate(&handle1);
  EXPECT_TRUE(s.ok()) << s.ToString();

  rocksdb::Options options;
  std::string value;
  for (int i = 0; i < 4; i++) {
    std::string key = "key" + std::to_string(i);
    std::string expect_value = "value" + std::to_string(i);
    s = ctx_->batch->GetFromBatch(options, key, &value);
    EXPECT_TRUE(s.ok()) << s.ToString();
    EXPECT_EQ(expect_value, value);
  }

  s = storage_->GetDB()->Write(rocksdb::WriteOptions(), ctx_->batch->GetWriteBatch());
  EXPECT_TRUE(s.ok()) << s.ToString();

  batch.Clear();
  s = batch.Delete("key0");
  EXPECT_TRUE(s.ok()) << s.ToString();

  s = batch.DeleteRange(storage_->GetCFHandle(ColumnFamilyID::PrimarySubkey), "key1", "key3");
  EXPECT_TRUE(s.ok()) << s.ToString();

  WriteBatchIndexer handle2(*ctx_);
  s = batch.Iterate(&handle2);
  s = batch.Iterate(&handle2);
  EXPECT_TRUE(s.ok()) << s.ToString();

  for (int i = 0; i < 3; i++) {
    std::string key = "key" + std::to_string(i);
    s = ctx_->batch->GetFromBatchAndDB(storage_->GetDB(), rocksdb::ReadOptions(), key, &value);
    EXPECT_TRUE(s.IsNotFound());
  }

  s = ctx_->batch->GetFromBatchAndDB(storage_->GetDB(), rocksdb::ReadOptions(), "key3", &value);
  EXPECT_TRUE(s.ok()) << s.ToString();
  EXPECT_EQ("value3", value);
}

TEST_F(WriteBatchIndexerTest, SingleDelete) {
  auto s = storage_->GetDB()->Put(rocksdb::WriteOptions(), "key", "value");
  EXPECT_TRUE(s.ok()) << s.ToString();

  std::string value;
  s = storage_->GetDB()->Get(rocksdb::ReadOptions(), "key", &value);
  EXPECT_TRUE(s.ok()) << s.ToString();
  EXPECT_EQ("value", value);

  rocksdb::WriteBatch batch;
  s = batch.SingleDelete("key");
  EXPECT_TRUE(s.ok()) << s.ToString();

  ctx_->batch = std::make_unique<rocksdb::WriteBatchWithIndex>();
  WriteBatchIndexer handle(*ctx_);
  s = batch.Iterate(&handle);
  EXPECT_TRUE(s.ok()) << s.ToString();

  s = ctx_->batch->GetFromBatchAndDB(storage_->GetDB(), rocksdb::ReadOptions(), "key", &value);
  EXPECT_TRUE(s.IsNotFound());
}

namespace {

struct CountingIndexHook : engine::IndexHook {
  int committed = 0;
  int last_batch_count = 0;

  void OnWriteCommitted(engine::Context& /*ctx*/, const rocksdb::WriteBatch& updates) override {
    committed++;
    last_batch_count = updates.Count();
  }
};

}  // namespace

TEST_F(WriteBatchIndexerTest, IndexHookNotifiedOnlyWhenCommitted) {
  CountingIndexHook hook;
  storage_->RegisterIndexHook(&hook);
  storage_->RegisterIndexHook(&hook);  // registering twice is a no-op
  EXPECT_EQ(storage_->IndexHookCount(), 1u);

  rocksdb::WriteBatch batch;
  auto s = batch.Put("key0", "value0");
  EXPECT_TRUE(s.ok()) << s.ToString();

  // Staging into the context batch does not commit anything, so hooks stay quiet.
  ctx_->batch = std::make_unique<rocksdb::WriteBatchWithIndex>();
  WriteBatchIndexer handle(*ctx_);
  s = batch.Iterate(&handle);
  EXPECT_TRUE(s.ok()) << s.ToString();
  EXPECT_EQ(hook.committed, 0);

  // Committing through Storage::Write notifies the hook with the committed batch.
  rocksdb::WriteBatch commit_batch;
  s = commit_batch.Put("key1", "value1");
  EXPECT_TRUE(s.ok()) << s.ToString();
  s = storage_->Write(*ctx_, rocksdb::WriteOptions(), &commit_batch);
  EXPECT_TRUE(s.ok()) << s.ToString();
  EXPECT_EQ(hook.committed, 1);
  EXPECT_GE(hook.last_batch_count, 1);

  storage_->UnregisterIndexHook(&hook);
  EXPECT_EQ(storage_->IndexHookCount(), 0u);

  rocksdb::WriteBatch another_batch;
  s = another_batch.Put("key2", "value2");
  EXPECT_TRUE(s.ok()) << s.ToString();
  s = storage_->Write(*ctx_, rocksdb::WriteOptions(), &another_batch);
  EXPECT_TRUE(s.ok()) << s.ToString();
  EXPECT_EQ(hook.committed, 1);
}
