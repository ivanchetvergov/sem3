// core/src/BigRingArithmetic.cc
#include "BigRingArithmetic.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

BigRingArithmetic::BigRingArithmetic(const FiniteRingRules& rules,
                                     const SmallRingArithmetic& small)
    : rules_(rules), small_(small) {}


// * --- СЛОЖЕНИЕ ---
RingNumber BigRingArithmetic::add(const RingNumber& a, const RingNumber& b) const {
    bool same_sign = (a.isNegative() == b.isNegative());

    RingNumber mag_a = a.withoutSign();
    RingNumber mag_b = b.withoutSign();

    if (same_sign) {
        RingNumber sum = addUnsigned(mag_a, mag_b);
        sum.setNegative(a.isNegative());
        return sum;
    }

    if (isGreaterOrEqual(mag_a, mag_b)) {
        RingNumber diff = subtractPositional(mag_a, mag_b);
        diff.setNegative(a.isNegative());
        return diff;
    }

    RingNumber diff = subtractPositional(mag_b, mag_a);
    diff.setNegative(b.isNegative());
    return diff;
}

RingNumber BigRingArithmetic::addUnsigned(const RingNumber& a, const RingNumber& b) const {
    size_t max_len = std::max(a.length(), b.length());
    
    std::vector<char> result_digits;
    result_digits.reserve(max_len + 1);
    
    char carry_out = zero_;
    
    for (size_t i = 0; i < max_len || carry_out != zero_; ++i) {
        char digit_a = a.getDigit(i);
        char digit_b = b.getDigit(i);
        char carry_in = carry_out;
        
        char sum_mid = small_.add(digit_a, digit_b);
        
        char carry_1 = zero_;
        if (isLessThan(sum_mid, digit_a)) {
            carry_1 = one_;
        }
        
        char sum_final = small_.add(sum_mid, carry_in);
        
        char carry_2 = zero_;
        if (carry_in != zero_ && isLessThan(sum_final, sum_mid)) {
            carry_2 = one_;
        }
        
        carry_out = (carry_1 != zero_ || carry_2 != zero_) ? one_ : zero_;
        result_digits.push_back(sum_final);
    }

    RingNumber result(rules_, result_digits);
    result.normalize();
    return result;
}

// * --- ВЫЧИТАНИЕ (через аддитивную инверсию) ---
RingNumber BigRingArithmetic::subtract(const RingNumber& a, const RingNumber& b) const {
    RingNumber neg_b = negate(b);
    return add(a, neg_b); // используем сложение с проверкой переполнения
}

// * --- ПОЗИЦИОННОЕ ВЫЧИТАНИЕ (для деления) ---
RingNumber BigRingArithmetic::subtractPositional(const RingNumber& a, const RingNumber& b) const {
    
    size_t max_len = std::max(a.length(), b.length());
    std::vector<char> result_digits;
    result_digits.reserve(max_len);
    
    char borrow = zero_; // заём
    
    for (size_t i = 0; i < max_len; ++i) {
        char digit_a = a.getDigit(i);
        char digit_b = b.getDigit(i);

        if (borrow != zero_) {
            digit_a = small_.subtract(digit_a, one_);
        }

        bool need_borrow = isLessThan(digit_a, digit_b);
        char diff = small_.subtract(digit_a, digit_b);

        result_digits.push_back(diff);
        borrow = need_borrow ? one_ : zero_;
    }

    if (borrow != zero_) {
        throw std::runtime_error("subtractPositional produced a negative result");
    }

    RingNumber result(rules_, result_digits);
    result.normalize();
    return result;
}

// * --- УМНОЖЕНИЕ ---
RingNumber BigRingArithmetic::multiply(const RingNumber& a, const RingNumber& b) const {
    RingNumber mag_a = a.withoutSign();
    RingNumber mag_b = b.withoutSign();
    
    if (mag_a.isZero() || mag_b.isZero()) {
        return RingNumber(rules_);
    }
    
    RingNumber result(rules_);
    
    for (size_t i = 0; i < mag_b.length(); ++i) {
        char digit_b = mag_b.getDigit(i);
        RingNumber partial = multiplyByDigit(mag_a, digit_b);
        partial = shiftLeft(partial, static_cast<int>(i));
        result = addUnsigned(result, partial);
    }
    
    bool negative = (a.isNegative() != b.isNegative());
    result.setNegative(negative);
    result.normalize();
    return result;
}

// * --- ДЕЛЕНИЕ С ОСТАТКОМ (Деление Столбиком) ---
DivisionResult BigRingArithmetic::divide(const RingNumber& a, const RingNumber& b) const {
    if (b.isZero()) {
        throw std::runtime_error("Division by zero_");
    }

    RingNumber dividend = a.withoutSign();
    RingNumber divisor = b.withoutSign();
    RingNumber quotient(rules_);
    RingNumber remainder = dividend;

    if (isGreaterOrEqual(dividend, divisor)) {
        int shift_amount = static_cast<int>(dividend.length()) - static_cast<int>(divisor.length());
        RingNumber shifted_divisor = shiftLeft(divisor, shift_amount);

        for (int i = shift_amount; i >= 0; --i) {
            char q_digit = findQuotientDigit(remainder, shifted_divisor);

            if (q_digit != zero_) {
                RingNumber q_digit_num(rules_, std::string(1, q_digit));
                RingNumber partial_quotient = shiftLeft(q_digit_num, i);
                quotient = addUnsigned(quotient, partial_quotient);

                RingNumber subtraction_amount = multiplyByDigit(shifted_divisor, q_digit);
                remainder = subtractPositional(remainder, subtraction_amount);
            }

            if (i > 0) {
                shifted_divisor = shiftRight(shifted_divisor, 1);
            }
        }
    }

    remainder.normalize();
    quotient.normalize();

    bool dividend_negative = a.isNegative();
    bool divisor_negative = b.isNegative();

    if (dividend_negative && !divisor_negative && !remainder.isZero()) {
        RingNumber one_num(rules_, std::string(1, one_));
        quotient = addUnsigned(quotient, one_num);
        remainder = subtractPositional(divisor, remainder);
    }

    bool quotient_negative = (dividend_negative != divisor_negative);
    quotient.setNegative(quotient_negative);

    bool remainder_negative = divisor_negative && !remainder.isZero();
    remainder.setNegative(remainder_negative);

    remainder.normalize();
    quotient.normalize();

    return DivisionResult(quotient, remainder);
}

// * --- АДДИТИВНАЯ ИНВЕРСИЯ (отрицание) ---
RingNumber BigRingArithmetic::negate(const RingNumber& a) const {
    RingNumber result = a;
    result.flipSign();
    result.normalize();
    return result;
}

// * --- ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ---
RingNumber BigRingArithmetic::multiplyByDigit(const RingNumber& num, char digit) const {

    if (digit == zero_ || num.isZero()) {
        return RingNumber(rules_);
    }

    RingNumber positive = num.withoutSign();

    if (digit == one_) {
        return positive;
    }

    RingNumber result(rules_);
    char counter = zero_;

    while (counter != digit) {
        result = addUnsigned(result, positive);
        counter = small_.plusOne(counter);
    }

    return result;
}

RingNumber BigRingArithmetic::shiftLeft(const RingNumber& num, int positions) const {
    if (positions == 0 || num.isZero()) {
        return num;
    }
    std::vector<char> result_digits;
    result_digits.reserve(num.length() + positions);

    // добавляем нули в младшие разряды
    for (int i = 0; i < positions; ++i) {
        result_digits.push_back(zero_);
    }

    // копируем цифры числа
    for (size_t i = 0; i < num.length(); ++i) {
        result_digits.push_back(num.getDigit(i));
    }

    RingNumber result(rules_, result_digits);
    result.setNegative(num.isNegative());
    return result;
}

RingNumber BigRingArithmetic::shiftRight(const RingNumber& num, int positions) const {
    if (positions <= 0 || num.isZero() || positions >= num.length()) {
        return RingNumber(rules_); // возвращаем ноль
    }

    std::vector<char> original_digits = num.toVector();
    std::vector<char> result_digits;
    
    // копируем цифры, начиная с positions (отбрасывая младшие разряды)
    for (size_t i = positions; i < original_digits.size(); ++i) {
        result_digits.push_back(original_digits[i]);
    }

    RingNumber result(rules_, result_digits);
    result.setNegative(num.isNegative());
    result.normalize(); 
    return result;
}

char BigRingArithmetic::findQuotientDigit(const RingNumber& remainder, const RingNumber& shifted_divisor) const {

    char q_digit = zero_;
    RingNumber temp_remainder = remainder;
    while (isGreaterOrEqual(temp_remainder, shifted_divisor)) {
        temp_remainder = subtractPositional(temp_remainder, shifted_divisor);
        q_digit = small_.plusOne(q_digit);
    }
    return q_digit;
}