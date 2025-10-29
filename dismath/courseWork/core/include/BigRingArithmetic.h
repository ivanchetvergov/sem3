#pragma once
#include "FiniteRingRules.h"
#include "SmallRingArithmetic.h"
#include "RingNumber.h"

class BigRingArithmetic {
public:
    BigRingArithmetic(const FiniteRingRules& rules, 
                       const SmallRingArithmetic& small);
    
    // * орперации: строка × строка → строка
    RingNumber add(const RingNumber& a, const RingNumber& b) const;
    RingNumber subtract(const RingNumber& a, const RingNumber& b) const;
    RingNumber multiply(const RingNumber& a, const RingNumber& b) const;
    RingNumber divide(const RingNumber& a, const RingNumber& b) const;

private:
    const FiniteRingRules& rules_;
    const SmallRingArithmetic& small_;
    
    // * вспомогательные методы
    RingNumber multiplyByDigit(const RingNumber& num, char digit) const;
    RingNumber shiftLeft(const RingNumber& num, int positions) const;
    // RingNumber negate(const RingNumber& a) const;
    friend bool isGreaterOrEqual(const RingNumber& a, const RingNumber& b, 
                      const FiniteRingRules& rules);
};