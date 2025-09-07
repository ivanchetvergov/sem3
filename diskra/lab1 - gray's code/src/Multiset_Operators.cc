// Multiset_Operators.cc
#include "Multiset.h"

// (базовый метод)
Multiset Multiset::unionWith(const Multiset& other) const {
    Multiset result = *this;
    for (const auto& pair : other.elements_) {
        const std::string& code = pair.first;
        const int other_cardinality = pair.second;
        result.elements_[code] = std::max(result.elements_[code], other_cardinality);
    }
    result.totalCardinality_ = 0;
    for (const auto& pair : result.elements_) {
        result.totalCardinality_ += pair.second;
    }
    return result;
}

// (базовый метод)
Multiset Multiset::difference(const Multiset& other) const {
    Multiset result = *this;
    for (const auto& pair : other.elements_) {
        const std::string& code = pair.first;
        const int other_cardinality = pair.second;
        if (result.elements_.count(code)) {
            result.elements_[code] -= other_cardinality;
            if (result.elements_[code] <= 0) {
                result.elements_.erase(code);
            }
        }
    }
    result.totalCardinality_ = 0;
    for (const auto& pair : result.elements_) {
        result.totalCardinality_ += pair.second;
    }
    return result;
}

// пересечение: A \ (A \ B)
Multiset Multiset::intersectWith(const Multiset& other) const {
    return this->difference(this->difference(other));
}

// см разность: (A \ B) or (B \ A)
// A ∆ B = (A \ B) ∪ (B \ A)
Multiset Multiset::symmetricDifference(const Multiset& other) const {
    Multiset a_minus_b = this->difference(other);
    Multiset b_minus_a = other.difference(*this);
    return a_minus_b.unionWith(b_minus_a);
}

// дополнение: U \ A
Multiset Multiset::complement(const Multiset& universe) const {
    return universe.difference(*this);
}

// -- (обертки) ---

Multiset Multiset::operator|(const Multiset& other) const {
    return this->unionWith(other);
}

Multiset Multiset::operator&(const Multiset& other) const {
    return this->intersectWith(other);
}

Multiset Multiset::operator-(const Multiset& other) const {
    return this->difference(other);
}

Multiset Multiset::operator^(const Multiset& other) const {
    return this->symmetricDifference(other);
}

// --- не множеств. оп-ии ---

Multiset Multiset::operator+(const Multiset& other) const {
    Multiset result = *this;
    for (const auto& pair : other.elements_) {
        const std::string& code = pair.first;
        const int other_cardinality = pair.second;
        result.elements_[code] += other_cardinality;
        result.totalCardinality_ += other_cardinality;
    }
    return result;
}

Multiset Multiset::operator*(const Multiset& other) const {
    Multiset result;
    for (const auto& pair_a : this->elements_) {
        const std::string& code_a = pair_a.first;
        const int a_cardinality = pair_a.second;
        if (other.elements_.count(code_a)) {
            const int b_cardinality = other.elements_.at(code_a);
            result.elements_[code_a] = a_cardinality * b_cardinality;
        }
    }
    result.totalCardinality_ = 0;
    for (const auto& pair : result.elements_) {
        result.totalCardinality_ += pair.second;
    }
    return result;
}

Multiset Multiset::operator/(const Multiset& other) const {
    Multiset result;
    for (const auto& pair_a : this->elements_) {
        const std::string& code_a = pair_a.first;
        const int a_cardinality = pair_a.second;
        if (other.elements_.count(code_a) && other.elements_.at(code_a) != 0) {
            const int b_cardinality = other.elements_.at(code_a);
            result.elements_[code_a] = a_cardinality / b_cardinality;
        }
    }
    result.totalCardinality_ = 0;
    for (const auto& pair : result.elements_) {
        result.totalCardinality_ += pair.second;
    }
    return result;
}
