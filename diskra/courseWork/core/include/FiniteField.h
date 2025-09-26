#pragma once
#include "FiniteFieldRules.h"

class FiniteField {
public:
    explicit FiniteField(const FiniteFieldRules& r) : rules_(r) {}

    char add(char a, char b) const;
    char subtract(char a, char b) const;
    char multiply(char a, char b) const;
    char divide(char a, char b) const;

    const FiniteFieldRules& getRules() const { return rules_; }

private:
    const FiniteFieldRules& rules_;
};
