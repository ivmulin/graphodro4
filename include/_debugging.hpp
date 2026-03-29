#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cstddef>
#include <iostream>
#include <vector>

/**
 * @brief Generic utility to print a vector in { a_0, a_1, ..., a_n } format.
 * @tparam T The type of elements in the vector.
 */
template <typename T>
void print_vector(const std::vector<T>& vec, std::ostream& os = std::cout) {
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
}

#endif  // HELPERS_HPP
