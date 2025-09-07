// Multiset_Helpers.cc
#include "Multiset.h"

void Multiset::print() const {
    if (isEmpty()) {
        std::cout << "мультимножество пустое.\n";
        return;
    }
    std::cout << "элементы мультимножества:\n";
    for (const auto& pair : elements_) {
        std::cout << "  " << pair.first << ": " << pair.second << "\n";
    }
    std::cout << "общая мощность: " << totalCardinality_ << "\n";
}

bool Multiset::isEmpty() const {
    return elements_.empty();
}