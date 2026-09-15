
#ifndef APPLIED_MECHS_UTILS_H
#define APPLIED_MECHS_UTILS_H
#include <iostream>
#include <ostream>
#include <string>

#include <type_traits>

inline void LOG_WARNING(std::string msg) noexcept {
    std::cerr << msg << std::endl;
}

template <typename T>
bool isFractional(T x) noexcept requires std::is_floating_point_v<T> {
    return std::trunc(x) != x;
}


#endif //APPLIED_MECHS_UTILS_H
