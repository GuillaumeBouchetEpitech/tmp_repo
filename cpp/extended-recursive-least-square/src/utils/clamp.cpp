
#include "clamp.hpp"

#include <algorithm>

namespace utils {

float clamp(float inValue, float minValue, float maxValue) { return std::min(std::max(inValue, minValue), maxValue); }

};