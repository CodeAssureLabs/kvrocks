#include <thread>

#include "storage/storage.h"

namespace engine {

void StartCompactionHelper(Storage *storage) {
  std::thread([storage] { (void)storage; }).detach();
}

}  // namespace engine
