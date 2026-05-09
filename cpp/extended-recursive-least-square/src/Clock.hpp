
#pragma once

#include <chrono>

class Clock {
public:
  Clock() = default;
  ~Clock() = default;

public:
  void start();
  void stop();
  int32_t getDuration() const;

private:
  std::chrono::high_resolution_clock::time_point _start;
  std::chrono::high_resolution_clock::time_point _stop;
  int32_t _duration = -1;
};
