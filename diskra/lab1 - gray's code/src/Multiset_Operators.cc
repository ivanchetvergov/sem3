// Multiset_Operators.cc
#include "Multiset.h"
#include "Exceptions.h"
#include <algorithm>

// (базовый метод) A or B 
Multiset Multiset::unionWith(const Multiset& other) const {
    Multiset result = *this;
    for (const auto& pair : other.elements_) {
        const std::string& code = pair.first;
        const int other_cardinality = pair.second;
        result.elements_[code] += other_cardinality;
    }
    result.totalCardinality_ = 0;
    for (const auto& pair : result.elements_) {
        result.totalCardinality_ += pair.second;
    }
    return result;
}

// (базовый метод) A \ B (Правильная разность мультимножеств)
// TODO: A and not B
Multiset Multiset::difference(const Multiset& other) const {
    Multiset result = *this; // Создаем копию A
    for (const auto& pair : other.elements_) { // перебираем B
        const std::string& code = pair.first;
        const int other_cardinality = pair.second;
        if (result.elements_.count(code)) { // если этот эл есть и в A
            int current_cardinality = result.elements_[code];
            int new_cardinality = current_cardinality - other_cardinality;
            if (new_cardinality > 0) { // если разность > 0
                result.elements_[code] = new_cardinality;
            } else { // если меньше 0
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

// пересечение: A and B = A \ (A \ B)
Multiset Multiset::intersectWith(const Multiset& other) const {
    Multiset a_minus_b = this->difference(other);
    return this->difference(a_minus_b);
}

// сим разность: (A \ B) or (B \ A)
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
    }
    result.totalCardinality_ = 0;
    for (const auto& pair : result.elements_) {
        result.totalCardinality_ += pair.second;
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
        if (other.elements_.count(code_a)) {
            const int b_cardinality = other.elements_.at(code_a);
            if (b_cardinality == 0) {
                throw InvalidOperationException("деление на 0 для элемента " + code_a);
            }
            result.elements_[code_a] = a_cardinality / b_cardinality;
        }
    }
    result.totalCardinality_ = 0;
    for (const auto& pair : result.elements_) {
        result.totalCardinality_ += pair.second;
    }
    return result;
}

Multiset Multiset::arithmeticDifference(const Multiset& other) const {
    Multiset result = *this; // создаем копию A
    
    // итерируемся по элементам B
    for (const auto& pair : other.elements_) {
        const std::string& code = pair.first;
        const int other_cardinality = pair.second;
        
        // вычитаем кратность B из кратности A
        result.elements_[code] -= other_cardinality;
    }

    // пересчитываем общую мощность
    result.totalCardinality_ = 0;
    for (const auto& pair : result.elements_) {
        result.totalCardinality_ += pair.second;
    }
    return result;
}