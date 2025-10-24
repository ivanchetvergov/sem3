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
    
    // * --- 1 определяем максимальную длину
    size_t max_len = std::max(a.length(), b.length());
    
    std::vector<char> result_digits;
    result_digits.reserve(max_len + 1);  // +1 для возможного переноса
    
    char carry = zero;  // Перенос
    
    // * --- 2 складываем поразрядно (от младших к старшим)
    for (size_t i = 0; i < max_len || carry != zero; ++i) {
        char digit_a = a.getDigit(i);  // Безопасный доступ
        char digit_b = b.getDigit(i);
        
        // * --- 3 считаем сумму текущих разрядов + перенос
        char sum = small_.add(digit_a, digit_b);
        char sum_with_carry = small_.add(sum, carry);
        
        // !"проверка переноса" суммой индексов
        int val_sum = rules_.getCharValue(sum_with_carry);
        int val_a = rules_.getCharValue(digit_a);
        int val_b = rules_.getCharValue(digit_b);
        int val_carry = rules_.getCharValue(carry);
        
        // вычисляем реальную сумму индексов
        int total = val_a + val_b + val_carry;
        int n = rules_.getSize();
        
        // если сумма >= размера поля, есть перенос
        if (total >= n) {
            carry = rules_.getOneElement();  // перенос = 1
        } else {
            carry = zero;
        }
        
        result_digits.push_back(sum_with_carry);
    }
    
    // * --- 4 формируем результат
    RingNumber result(rules_, result_digits);
    result.normalize();
    
    return result;
}

RingNumber BigRingArithmetic::subtract(const RingNumber& a, const RingNumber& b) const {
    const char zero = rules_.getZeroElement();
    
    size_t max_len = std::max(a.length(), b.length());
    
    std::vector<char> result_digits;
    result_digits.reserve(max_len);
    
    char borrow = zero;  // заём
    
    // вычитаем поразрядно
    for (size_t i = 0; i < max_len; ++i) {
        char digit_a = a.getDigit(i);
        char digit_b = b.getDigit(i);
        
        // вычитаем через SmallRingArithmetic
        char diff = small_.subtract(digit_a, digit_b);
        diff = small_.subtract(diff, borrow);
        
        // проверяем заём
        int val_a = rules_.getCharValue(digit_a);
        int val_b = rules_.getCharValue(digit_b);
        int val_borrow = rules_.getCharValue(borrow);
        int n = rules_.getSize();
        
        int total = val_a - val_b - val_borrow;
        
        if (total < 0) {
            borrow = rules_.getOneElement();  // заём из старшего разряда
        } else {
            borrow = zero;
        }
        
        result_digits.push_back(diff);
    }
    
    RingNumber result(rules_, result_digits);
    result.normalize();
    
    return result;
}

RingNumber BigRingArithmetic::multiply(const RingNumber& a, const RingNumber& b) const {
    const char zero = rules_.getZeroElement();
    
    // умножение на ноль
    if (a.isZero() || b.isZero()) {
        return RingNumber(rules_);
    }
    
    // * умножение "столбиком"
    RingNumber result(rules_);  // начинаем с нуля
    
    // для каждой цифры второго числа
    for (size_t i = 0; i < b.length(); ++i) {
        char digit_b = b[i];
        
        // умножаем первое число на эту цифру
        RingNumber partial = multiplyByDigit(a, digit_b);
        
        // сдвигаем результат на i позиций влево
        partial = shiftLeft(partial, i);
        
        // добавляем к общему результату
        result = add(result, partial);
    }
    
    result.normalize();
    return result;
}

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
    RingNumber quotient(rules_); // Начинаем с нуля
    RingNumber one(rules_, std::string(1, rules_.getOneElement())); 
    
    // * пока остаток >= делитель
    while (isGreaterOrEqual(remainder, b, rules_)) {
        // вычитаем Делитель из Остатка
        remainder = subtract(remainder, b);
        // увеличиваем частное на 1
        quotient = add(quotient, one);
        
    }
    
    return quotient;
}


bool isGreaterOrEqual(const RingNumber& a, const RingNumber& b, 
                      const FiniteRingRules& rules) {
    
    size_t len_a = a.length();
    size_t len_b = b.length();
    
    // * --- 1 Сравнение по длине
    if (len_a != len_b) {
        return len_a > len_b; // Более длинное число всегда больше
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

RingNumber BigRingArithmetic::multiplyByDigit(const RingNumber& num, char digit) const {
    const char zero = rules_.getZeroElement();
    const char one = rules_.getOneElement();
    
    if (digit == zero || num.isZero()) {
        return RingNumber(rules_);
    }
    
    // оптимизация для умножения на 1
    if (digit == one) {
        return num;
    }
    
    // digit_value раз прибавляем num к результату
    int digit_value = rules_.getCharValue(digit);
    
    RingNumber result(rules_);
    
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
    
    // копируем цифры числа
    for (size_t i = 0; i < num.length(); ++i) {
        result_digits.push_back(num[i]);
    }
    
    RingNumber result(rules_, result_digits);
    return result;
}