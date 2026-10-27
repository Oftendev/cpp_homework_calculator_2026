#include "plugin_api.h"
#include <cmath>
#include <limits>

extern "C" PLUGIN_API const char* GetFunctionName() {
    return "ln";
}

extern "C" PLUGIN_API double Compute(double arg) {
    // Вместо исключения возвращаем NaN
    if (arg <= 0.0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return std::log(arg);
}