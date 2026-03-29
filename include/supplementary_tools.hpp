#ifndef SUPPLEMENTARY_TOOLS_HPP
#define SUPPLEMENTARY_TOOLS_HPP

#include <vector>

template <typename T>
int count_occurrences(const std::vector<T>& vector, T value);

template <typename T>
bool contains(const std::vector<T>& vector, T value);

size_t removeFirstN(std::vector<size_t>& vector, size_t target,
                    size_t occurrences);

#endif  // SUPPLEMENTARY_TOOLS_HPP
