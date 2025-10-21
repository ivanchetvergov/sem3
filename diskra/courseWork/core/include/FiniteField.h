#pragma once
#include "FiniteFieldRules.h"

class FiniteField {
public:
    explicit FiniteField(const FiniteFieldRules& r) : rules_(r) {}

    // * арифметические операции в поле
    char add(char a, char b) const;
    char subtract(char a, char b) const;
    char multiply(char a, char b) const;
    char divide(char a, char b) const;

    // * получить правила поля
    const FiniteFieldRules& getRules() const { return rules_; }
    // * найти мультипликативный обратный элемент
    char findMultiplicativeInverse(char element) const;

private:
    //  ссылка на правила поля
    const FiniteFieldRules& rules_;
};
