#include "FiniteField.h"
#include <stdexcept>

FiniteField::FiniteField(const FiniteFieldRules& rules) : rules_(rules) {
    precomputeInverses();
}

char FiniteField::add(char a, char b) const {
    int val_a = rules_.getCharValue(a);
    int val_b = rules_.getCharValue(b);
    int sum_val = (val_a + val_b) % rules_.getSize();
    return rules_.getValueChar(sum_val);
}

char FiniteField::subtract(char a, char b) const {
    return add(a, findAdditiveInverse(b));
}

char FiniteField::multiply(char a, char b) const {
    int val_a = rules_.getCharValue(a);
    int val_b = rules_.getCharValue(b);
    int prod_val = (val_a * val_b) % rules_.getSize();
    return rules_.getValueChar(prod_val);
}

char FiniteField::divide(char a, char b) const {
    if (b == rules_.getZeroElement())
        throw std::runtime_error("division by zero element");
    return multiply(a, findMultiplicativeInverse(b));
}

char FiniteField::findAdditiveInverse(char val) const {
    auto it = additive_inverses_.find(val);
    if (it == additive_inverses_.end())
        throw std::runtime_error("additive inverse not found");
    return it->second;
}

char FiniteField::findMultiplicativeInverse(char val) const {
    auto it = multiplicative_inverses_.find(val);
    if (it == multiplicative_inverses_.end())
        throw std::runtime_error("multiplicative inverse not found");
    return it->second;
}

void FiniteField::precomputeInverses() {
    int size = rules_.getSize();
    char zero = rules_.getZeroElement();
    char one = rules_.getOneElement();

    for (int i = 0; i < size; ++i) {
        char c = rules_.getValueChar(i);
        additive_inverses_[c] = rules_.getValueChar((size - i) % size);
        if (c != zero) {
            // простой перебор для мультипликативного обратного
            for (int j = 0; j < size; ++j) {
                char candidate = rules_.getValueChar(j);
                if (candidate != zero && multiply(c, candidate) == one) {
                    multiplicative_inverses_[c] = candidate;
                    break;
                }
            }
        }
    }
}

const FiniteFieldRules& FiniteField::getRules() const{
    return rules_;
}
