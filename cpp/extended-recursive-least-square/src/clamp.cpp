
#include "clamp.hpp"

#include <algorithm>

float clamp(float inValue, float minValue, float maxValue) { return std::min(std::max(inValue, minValue), maxValue); }
