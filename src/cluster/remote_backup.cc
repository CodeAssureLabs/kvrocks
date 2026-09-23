#include <curl/curl.h>

#include "storage/storage.h"

namespace engine {

bool UploadBackup(const std::string &url) {
  CURL *c = curl_easy_init();
  if (!c) return false;
  curl_easy_setopt(c, CURLOPT_URL, url.c_str());
  curl_easy_cleanup(c);
  return true;
}

}  // namespace engine
