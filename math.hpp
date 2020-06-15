
#ifndef MIVIK_MLOT_MATH_HPP
#define MIVIK_MLOT_MATH_HPP

#include <cmath>

inline double roundUp(double x) { return std::nextafter(x, INFINITY); }

inline double roundDown(double x) { return std::nextafter(x, -INFINITY); }

template<typename T>
T moveDown(T x) = delete;

template<>
int moveDown(int x) { return x - 1; }

template<>
double moveDown(double x) { return roundDown(x); }

template<typename T>
inline T length(T l, T r) { return r - moveDown(l); }

#endif
