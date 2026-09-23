#include "storage/storage.h"
#include "task_runner.h"

namespace engine {

Status StartCompactionHelper(TaskRunner *task_runner, Storage *storage) {
  return task_runner->TryPublish([storage] { (void)storage; });
}

}  // namespace engine
