
#include "Clock.hpp"

#include <chrono>
#include <utility> // std::forward

namespace {

#define D_ALIAS_FUNCTION(__new_func_name, __existing_func_name)                                                        \
  template <typename... Args>                                                                                          \
  auto __new_func_name(Args&&... args) -> decltype(__existing_func_name(std::forward<Args>(args)...)) {                \
    return __existing_func_name(std::forward<Args>(args)...);                                                          \
  }

D_ALIAS_FUNCTION(_getTime, std::chrono::high_resolution_clock::now);
D_ALIAS_FUNCTION(_asMilliSeconds, std::chrono::duration_cast<std::chrono::milliseconds>);
D_ALIAS_FUNCTION(_asNanoSeconds, std::chrono::duration_cast<std::chrono::nanoseconds>);

} // namespace

//
//
//
//
//

namespace utils {

void Clock::start() { _start = _getTime(); }

void Clock::stop() {
  _stop = _getTime();
  if (_start > _stop) {
    _duration = -1;
  } else {
    _duration = int32_t(_asNanoSeconds(_stop - _start).count());
  }
}

int32_t Clock::getDuration() const { return _duration; }

};
