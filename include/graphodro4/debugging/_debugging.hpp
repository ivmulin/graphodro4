#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cstdarg>
#include <cstddef>
#include <iostream>
#include <set>
#include <vector>

namespace debugging {

/**
 * @brief Generic utility to print a vector in { a_0, a_1, ..., a_n } format.
 * @tparam T The type of elements in the vector.
 */
template <typename T>
void print_vector(const std::vector<T>& vec, bool newline = true,
                  std::ostream& os = std::cout) {
    os << "{ ";
    if (!vec.empty()) {
        for (size_t i = 0; i < vec.size(); ++i) {
            os << vec[i];
            if (i != vec.size() - 1) {
                os << ", ";
            }
        }
        os << " ";
    }
    os << "}";
    if (newline) {
        os << "\n";
    }
}

template <typename T>
void print_set(const std::set<T>& s, bool newline = true,
               std::ostream& os = std::cout) {
    os << "< ";
    if (!s.empty()) {
        for (const auto& elem : s) {
            os << elem;
            if (&elem != &*s.rbegin()) {
                os << ", ";
            }
        }
        os << " ";
    }
    os << ">";
    if (newline) {
        os << "\n";
    }
}

inline void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

}  // namespace debugging

#endif  // HELPERS_HPP
