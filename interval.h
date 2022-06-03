#ifndef INTERVAL_H
#define INTERVAL_H

#include <iostream>
#include "general.h"

class interval {
  public:
    interval() : min(+inf), max(-inf) {} // Default interval is empty

    interval(double _min, double _max) : min(_min), max(_max) {}

    interval(const interval& a, const interval& b)
      : min(fmin(a.min, b.min)), max(fmax(a.max, b.max)) {}

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    static const interval empty, universe;

  public:
    double min, max;
};

const interval interval::empty    = interval(+inf, -inf);
const interval interval::universe = interval(-inf, +inf);

interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
}
interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}
inline bool operator==(const interval& val1, const interval& val2){
    return (val1.min==val2.min) && (val1.max==val2.max);
}
inline std::ostream& operator<<(std::ostream &out, const interval& ival) {
    return out << ival.min << ' ' << ival.max << ' ';
}

#endif