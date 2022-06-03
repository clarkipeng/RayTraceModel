#ifndef GENERAL_H
#define GENERAL_H


#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
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
inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}
inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}
inline int random_int(int min, int max) {//inclusive
    return min + rand()%(max-min+1);
}
inline long long random_ll(long long min, long long max) {//inclusive
    return min + rand()%(max-min+1);
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
