#include "Global.h"
// std
#include <cmath>

float Global::log_speed(float speed)
{
    return std::log2(3 * speed + 1) / 2;
}
