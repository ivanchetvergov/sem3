#pragma once
#include "FiniteFieldRules.h"
#include <map>

class FiniteField {
public:
    explicit FiniteField(const FiniteFieldRules& rules);

    char add(char a, char b) const;
    char subtract(char a, char b) const;
    char multiply(char a, char b) const;
    char divide(char a, char b) const;

    const FiniteFieldRules& getRules() const;

private:
    char findAdditiveInverse(char val) const;
    char findMultiplicativeInverse(char val) const;

    const FiniteFieldRules& rules_;
    std::map<char, char> additive_inverses_;
    std::map<char, char> multiplicative_inverses_;
    void precomputeInverses();
};
