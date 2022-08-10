#include "GtiTLS.h"
#include <atomic>
#include <cassert>
#include <list>
#include <mutex>
#include <stdio.h>

static std::list<int> oldIds{};
static std::atomic<int> maxId{0};
static std::mutex m;

class TLSid {
  int tid{-1};
  void init() {
    std::lock_guard<std::mutex> lock(m);
    if (oldIds.empty()) {
      tid = maxId++;
    } else {
      tid = oldIds.back();
      oldIds.pop_back();
    }
  }

public:
  TLSid() { init(); }
  ~TLSid() {
    std::lock_guard<std::mutex> lock(m);
    oldIds.push_back(tid);
  }
  int getTid() {
    assert(tid >= 0);
    return tid;
  }
};

static thread_local TLSid tid{};
int getGtiTid() { return tid.getTid(); }
