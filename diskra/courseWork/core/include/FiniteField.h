// core/include/FiniteField.h
#pragma once

#include "FiniteFieldRules.h"
#include <string>
#include <vector>

class FiniteField {
public:
    explicit FiniteField(const FiniteFieldRules& rules);

    char add(char a, char b) const;
    char subtract(char a, char b) const;
    char multiply(char a, char b) const;
    char divide(char a, char b) const;
    
    char power(char base, int exponent) const;

    char find_additive_inverse(char val) const;
    char find_multiplicative_inverse(char val) const;

private:
    const FiniteFieldRules& rules_;
    
    std::map<char, char> additive_inverses_;
    std::map<char, char> multiplicative_inverses_;

    void precompute_inverses();
};