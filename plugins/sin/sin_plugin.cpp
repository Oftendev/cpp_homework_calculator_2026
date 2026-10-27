#include "plugin_api.h"
#include <cmath>

extern "C" PLUGIN_API const char* GetFunctionName() {
    return "sin";
}

// Синус принимает градусы, поэтому переводим в радианы.
extern "C" PLUGIN_API double Compute(double arg) {
    constexpr double PI = 3.14159265358979323846;
    return std::sin(arg * PI / 180.0);
}