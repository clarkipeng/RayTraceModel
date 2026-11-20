#ifndef GENERAL_H
#define GENERAL_H


#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include <cstdint>
#include <functional>
#include <thread>
#include <math.h>

using std::shared_ptr;
using std::make_shared;

const double inf = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double radians(double degrees) {
    return degrees * pi / 180.0;
}
inline double degrees(double radians) {
    return radians * 180.0 / pi;
}
// inline int random_int(int lowerbound, int upperbound) { //inclusive exclusive
//     return lowerbound + rand() %(upperbound-lowerbound+1);
// }
// Per-thread xorshift64*: glibc rand() takes a global lock, which made the render threads stall on each other.
inline uint64_t random_u64() {
    thread_local uint64_t state = 0x9E3779B97F4A7C15ULL ^ (uint64_t)std::hash<std::thread::id>()(std::this_thread::get_id()) * 0xD1B54A32D192ED03ULL;
    state ^= state >> 12; state ^= state << 25; state ^= state >> 27;
    return state * 0x2545F4914F6CDD1DULL;
}
inline double random_double() {
    return (random_u64() >> 11) * (1.0 / 9007199254740992.0);
}
inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}
inline int random_int(int min, int max) {//inclusive
    return min + (int)(random_u64() % (uint64_t)(max-min+1));
}
inline long long random_ll(long long min, long long max) {//inclusive
    return min + (long long)(random_u64() % (uint64_t)(max-min+1));
}
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}


#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif
