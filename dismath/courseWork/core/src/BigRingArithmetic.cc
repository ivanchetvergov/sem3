// core/src/BigRingArithmetic.cc
#include "BigRingArithmetic.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

BigRingArithmetic::BigRingArithmetic(const FiniteRingRules& rules,
                                     const SmallRingArithmetic& small)
    : rules_(rules), small_(small) {}

    
RingNumber BigRingArithmetic::add(const RingNumber& a, const RingNumber& b) const {
    const char zero = rules_.getZeroElement();
    
    size_t max_len = std::max(a.length(), b.length());
    
    std::vector<char> result_digits;
    result_digits.reserve(max_len + 1);
    
    char carry = zero;
    
    // * складываем поразрядно от младших к старшим
    for (size_t i = 0; i < max_len || carry != zero; ++i) {
        char digit_a = a.getDigit(i);
        char digit_b = b.getDigit(i);
        
        // * складываем через малую арифметику (правило +1)
        char sum = small_.add(digit_a, digit_b);
        sum = small_.add(sum, carry);
        
        // ! проверка переноса: если сумма индексов >= N - перенос
        int val_a = rules_.getCharValue(digit_a);
        int val_b = rules_.getCharValue(digit_b);
        int val_carry = rules_.getCharValue(carry);
        int n = rules_.getSize();
        
        int total = val_a + val_b + val_carry;
        
        if (total >= n) {
            carry = rules_.getOneElement();
            // carry = total >= 2 * n ? 
            //         rules_.getValueChar(2) : 
            //         rules_.getOneElement();
        } else {
            carry = zero;
        }
        
        result_digits.push_back(sum);
    }
    
    RingNumber result(rules_, result_digits);
    result.normalize();
    return result;
}

// TODO: написать тесты  !!!
RingNumber BigRingArithmetic::subtract(const RingNumber& a, const RingNumber& b) const {
    const char zero = rules_.getZeroElement();
    int n = rules_.getSize();

    size_t max_len = std::max(a.length(), b.length());
    std::vector<char> result_digits;
    result_digits.reserve(max_len);

    int borrow = 0; // заем

    // идём от младших к старшим (0 -> ...)
    for (size_t i = 0; i < max_len; ++i) {
        int val_a = rules_.getCharValue(a.getDigit(i));
        int val_b = rules_.getCharValue(b.getDigit(i));

        int diff = val_a - val_b - borrow;
        if (diff < 0) {
            diff += n;
            borrow = 1;
        } else {
            borrow = 0;
        }

        result_digits.push_back(rules_.getValueChar(diff));
    }

    // убираем ведущие нули в конце 
    while (result_digits.size() > 1 && result_digits.back() == zero) {
        result_digits.pop_back();
    }

    RingNumber result(rules_, result_digits);
    result.normalize();
    return result;
}

RingNumber BigRingArithmetic::multiply(const RingNumber& a, const RingNumber& b) const {
    const char zero = rules_.getZeroElement();
    
    if (a.isZero() || b.isZero()) {
        return RingNumber(rules_);
    }
    
    RingNumber result(rules_);
    
    // * умножение столбиком
    for (size_t i = 0; i < b.length(); ++i) {
        char digit_b = b.getDigit(i);
        
        RingNumber partial = multiplyByDigit(a, digit_b);
        partial = shiftLeft(partial, i);
        result = add(result, partial);
    }
    
    result.normalize();
    return result;
}

bool isGreaterOrEqual(const RingNumber& a, const RingNumber& b, 
                      const FiniteRingRules& rules) {
    
    size_t len_a = a.length();
    size_t len_b = b.length();
                        
    // * --- 1 Сравнение по длине
    if (len_a != len_b) {
        return len_a > len_b; // более длинное число всегда больше
    }
    
    // * --- 2 Длины равны, сравниваем поразрядно от старшего к младшему
    for (size_t i = len_a; i > 0; --i) {
        char digit_a = a.getDigit(i - 1); // старший разряд находится по индексу i-1
        char digit_b = b.getDigit(i - 1);
        
        int val_a = rules.getCharValue(digit_a);
        int val_b = rules.getCharValue(digit_b);
        
        if (val_a != val_b) {
            return val_a > val_b;
        }
    }
    return true; // числа равны
}

// TODO: написать тесты  !!! не уверен в общем случае
RingNumber BigRingArithmetic::divide(const RingNumber& a, const RingNumber& b) const {
    if (b.isZero()) {
        throw std::runtime_error("Division by zero in BigRingArithmetic");
    }
    
    // * простой случай: деление на однозначное число
    if (b.length() == 1) {
        char divisor = b[0];
        
        try {
            // находим обратный элемент
            char inv = small_.findMultiplicativeInverse(divisor);
            
            // умножаем на обратный
            RingNumber inv_num(rules_, std::string(1, inv));
            return multiply(a, inv_num);
        } catch (const std::runtime_error&) {
            throw std::runtime_error(
                "Cannot divide: divisor has no multiplicative inverse"
            );
        }
    }
    
    // * общий случай: деление многозначного числа на многозначное

    if (!isGreaterOrEqual(a, b, rules_)) {
        return RingNumber(rules_); // возвращаем ноль если a < b 
    }
    
    RingNumber remainder = a;
    RingNumber quotient(rules_); // начинаем с нуля
    RingNumber one(rules_, std::string(1, rules_.getOneElement())); 
    
    // * пока остаток >= делитель
    while (isGreaterOrEqual(remainder, b, rules_)) {
        // вычитаем делитель из остатка
        remainder = subtract(remainder, b);
        // увеличиваем частное на 1
        quotient = add(quotient, one);
        
    }
    
    return quotient;
}

RingNumber BigRingArithmetic::multiplyByDigit(const RingNumber& num, char digit) const {
    const char zero = rules_.getZeroElement();
    const char one = rules_.getOneElement();

    if (digit == zero || num.isZero()) {
        return RingNumber(rules_);
    }

    // если умножаем на 1 — возвращаем копию
    if (digit == one) {
        return num;
    }

    int digit_value = rules_.getCharValue(digit);
    RingNumber result(rules_);

    // на каждом шаге добавляем num
    for (int i = 0; i < digit_value; ++i) {
        result = add(result, num);
    }

    return result;
}

RingNumber BigRingArithmetic::shiftLeft(const RingNumber& num, int positions) const {
    if (positions == 0 || num.isZero()) {
        return num;
    }

    const char zero = rules_.getZeroElement();

    std::vector<char> result_digits;
    result_digits.reserve(num.length() + positions);

    // добавляем нули в младшие разряды
    for (int i = 0; i < positions; ++i) {
        result_digits.push_back(zero);
    }

    // копируем цифры числа от младшего к старшему 
    for (size_t i = 0; i < num.length(); ++i) {
        result_digits.push_back(num.getDigit(i));
    }

    RingNumber result(rules_, result_digits);
    return result;
}
