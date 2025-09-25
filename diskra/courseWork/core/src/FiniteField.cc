// "FiniteField.cc
#include "FiniteField.h"

FiniteField::FiniteField(const FiniteFieldRules& rules) : rules_(rules) {
    precompute_inverses();
}

char FiniteField::add(char a, char b) const {
    int val_a = rules_.getCharValue(a);
    int val_b = rules_.getCharValue(b);

    int sum_val = (val_a + val_b) % rules_.getSize();

    return rules_.getValueChar(sum_val);
}

char FiniteField::subtract(char a, char b) const {
    char inverse_b = additive_inverses_.at(b);
    return add(a, inverse_b);
}

char FiniteField::multiply(char a, char b) const {
    int val_a = rules_.getCharValue(a);
    int val_b = rules_.getCharValue(b);

    int product_val = (val_a * val_b) % rules_.getSize();

    return rules_.getValueChar(product_val);
}

char FiniteField::divide(char a, char b) const {
    if (rules_.getZeroElement() == b) {
        throw std::runtime_error("Division by zero element is not allowed.");
    }

    char inverse_b = find_multiplicative_inverse(b);
    return multiply(a, inverse_b);
}

char FiniteField::find_additive_inverse(char val) const {
    int val_int = rules_.getCharValue(val);
    int zero_int = rules_.getCharValue(rules_.getZeroElement());
    int inverse_int = (zero_int - val_int + rules_.getSize()) % rules_.getSize();
    return rules_.getValueChar(inverse_int);
}

char FiniteField::find_multiplicative_inverse(char val) const {
    if (val == rules_.getZeroElement()) {
        throw std::runtime_error("Cannot find multiplicative inverse of zero element.");
    }

    for (int i = 0; i < rules_.getSize(); ++i) {
        char current_char = rules_.getValueChar(i);
        if (multiply(val, current_char) == rules_.getOneElement()) {
            return current_char;
        }
    }
    throw std::runtime_error("Multiplicative inverse does not exist.");
}

void FiniteField::precompute_inverses() {
    for (int i = 0; i < rules_.getSize(); ++i) {
        char current_char = rules_.getValueChar(i);
        additive_inverses_[current_char] = find_additive_inverse(current_char);
        
        if (current_char != rules_.getZeroElement()) {
            try {
                multiplicative_inverses_[current_char] = find_multiplicative_inverse(current_char);
            } catch (const std::runtime_error& e) {

            }
        }
    }
}

const FiniteFieldRules& FiniteField::getRules() const {
    return rules_;
}

