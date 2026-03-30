#include "supplementary_tools.hpp"

template <typename T>
int count_occurrences(const std::vector<T>& vector, T value) {
    int count = 0;
    for (T v : vector) {
        if (v == value) {
            count++;
        }
    }
    return count;
}

template <typename T>
bool contains(const std::vector<T>& vector, T value) {
    for (T v : vector) {
        if (v == value) {
            return true;
        }
    }
    return false;
}

size_t removeFirstN(std::vector<size_t>& vector, size_t target,
                    size_t occurrences) {
    size_t count = 0;
    for (size_t i = 0; i < vector.size() && count < occurrences;) {
        if (vector[i] == target) {
            // Swap with the last element and remove it
            vector[i] = std::move(vector.back());
            vector.pop_back();
            count++;
            // Note: Don't increment 'i' here because the new
            // vec[i] (the former back element) needs to be checked
        } else {
            i++;
        }
    }
    return count;
}
