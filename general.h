#ifndef GENERAL_H
#define GENERAL_H


#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include <math.h>
#include <cstdint>

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
// xoshiro256+ with per-thread state: rand() shares one state across all render threads.
inline uint64_t *rng_state() {
    thread_local uint64_t s[4] = {0x9E3779B97F4A7C15ull, 0xBF58476D1CE4E5B9ull, 0x94D049BB133111EBull, 0x2545F4914F6CDD1Dull};
    return s;
}
inline void rng_seed(uint64_t seed) { // splitmix64 expands the seed into the state
    uint64_t *s = rng_state();
    for (int i = 0; i < 4; i++) {
        uint64_t z = (seed += 0x9E3779B97F4A7C15ull);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
        s[i] = z ^ (z >> 31);
    }
}
inline uint64_t rng_next() {
    uint64_t *s = rng_state();
    uint64_t r = s[0] + s[3], t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t;
    s[3] = (s[3] << 45) | (s[3] >> 19);
    return r;
}
inline double random_double() {
    return (rng_next() >> 11) * 0x1.0p-53;
}
inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}
inline int random_int(int min, int max) {//inclusive
    return min + rng_next()%(max-min+1);
}
inline long long random_ll(long long min, long long max) {//inclusive
    return min + rng_next()%(max-min+1);
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
