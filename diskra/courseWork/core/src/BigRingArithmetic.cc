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
    
    char carry_out = zero; // перенос
    
    // * складываем поразрядно от младших к старшим
    for (size_t i = 0; i < max_len || carry_out != zero; ++i) {
        char digit_a = a.getDigit(i);
        char digit_b = b.getDigit(i);
        char carry_in = carry_out; // Carry In = Carry Out предыдущей итерации
        
        // * --- 1 cложение A + B + C_in в один шаг
        char sum = small_.add(digit_a, digit_b);
        sum = small_.add(sum, carry_in);
        
        // * --- 2 вычисляем новый перенос (Carry Out)
        int val_a = rules_.getCharValue(digit_a);
        int val_b = rules_.getCharValue(digit_b);
        int val_carry_in = rules_.getCharValue(carry_in);
        int n = rules_.getSize();
        
        int total = val_a + val_b + val_carry_in; // временный 'int' для переноса
        
        if (total >= n) {
            carry_out = rules_.getOneElement();
        } else {
            carry_out = zero;
        }
        
        result_digits.push_back(sum); 
    }
    
    RingNumber result(rules_, result_digits);
    result.normalize();
    return result;
}
// TODO: написать тесты  !!!
RingNumber BigRingArithmetic::subtract(const RingNumber& a, const RingNumber& b) const {
    // --- 1  находим аддитивный инверс B
    RingNumber negated_b = negate(b);
    
    // --- 2 возвращаем результат сложения A + (-B)
    return add(a, negated_b);
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
                        
    // * --- 1 cравнение по длине
    if (len_a != len_b) {
        return len_a > len_b; // более длинное число всегда больше
    }
    
    // * --- 2 длины равны, сравниваем поразрядно от старшего к младшему
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

// TODO: реализовать остаток
DivisionResult BigRingArithmetic::divide(const RingNumber& a, const RingNumber& b) const {
    if (b.isZero()) {
        throw std::runtime_error("Division by zero in BigRingArithmetic");
    }

    // * --- 1. ПРОВЕРКА ОБРАТИМОСТИ СТАРШЕГО КОЭФФИЦИЕНТА
    // Деление многочленов (BigRingNumber) возможно только, если LC делителя обратим.
    char lc_b = b.leadingCoefficient();
    char lc_b_inv;
    try {
        lc_b_inv = small_.findMultiplicativeInverse(lc_b);
    } catch (const std::runtime_error&) {
        // Это ловит делители нуля, такие как 'c' (2) или 'g' (4) в Z8.
        throw std::runtime_error(
            "Division failed: Leading coefficient of divisor is not invertible (zero divisor)."
        );
    }

    // Если делимое меньше делителя, частное 0, остаток A.
    if (a.degree() < b.degree()) {
        return {RingNumber(rules_), a};
    }

    RingNumber quotient(rules_);
    RingNumber remainder = a;
    
    const char zero = rules_.getZeroElement();

    // * --- 2. АЛГОРИТМ ДЛИННОГО ДЕЛЕНИЯ (LONG DIVISION)
    
    // Цикл работает, пока остаток R больше или равен степени делителя B
    while (remainder.degree() >= b.degree() && !remainder.isZero()) {
        
        size_t degree_r = remainder.degree();
        size_t degree_b = b.degree();
        size_t k = degree_r - degree_b; // Степень сдвига (x^k)

        char lc_r = remainder.leadingCoefficient();
        
        // --- 2.1 Находим частное текущих старших коэффициентов: q_k = LC(R) / LC(B)
        // Деление равно умножению на обратный элемент: q_k = LC(R) * LC(B)^(-1)
        char q_k = small_.multiply(lc_r, lc_b_inv); 
        
        // --- 2.2 Формируем одночлен для вычитания: q_k * B * x^k
        RingNumber term = multiplyByDigit(b, q_k); // q_k * B
        term = shiftLeft(term, k); // (q_k * B) * x^k

        // --- 2.3 Вычитаем: R = R - term
        remainder = subtract(remainder, term);
        
        // --- 2.4 Добавляем q_k * x^k к частному
        
        // Создаем частное-одночлен с коэффициентом q_k и сдвигом k
        std::vector<char> q_k_digits(k + 1, zero);
        q_k_digits[k] = q_k; 
        
        RingNumber q_k_term(rules_, q_k_digits);
        
        // Складываем с общим частным
        quotient = add(quotient, q_k_term);
        
        // Нормализация остатка (важно, чтобы степень была пересчитана)
        remainder.normalize();
        
        // Если вычитание не уменьшило степень, это ошибка в логике сложения/вычитания
        if (degree_r == remainder.degree() && !remainder.isZero()) {
             // Предотвращение бесконечного цикла
             throw std::runtime_error("Division internal error: Subtraction did not reduce remainder degree.");
        }
    }
    
    // ! возвращаем результат
    return DivisionResult(quotient, remainder);
}

RingNumber BigRingArithmetic::multiplyByDigit(const RingNumber& num, char digit) const {
    const char zero = rules_.getZeroElement();
    const char one = rules_.getOneElement();

    if (digit == zero || num.isZero()) {
        return RingNumber(rules_);
    }

    if (digit == one) {
        return num;
    }

    RingNumber result(rules_);
    char counter = zero;

    // итерация `value(digit)` раз, используя только символы
    while (counter != digit) {
        result = add(result, num); 
        counter = small_.plusOne(counter); 
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

RingNumber BigRingArithmetic::negate(const RingNumber& a) const {
    std::vector<char> negated_digits;
    negated_digits.reserve(a.length()); 

    for (size_t i = 0; i < a.length(); ++i) {
        char digit = a.getDigit(i); 

        char negated_digit = small_.findAdditiveInverse(digit); 
        
        negated_digits.push_back(negated_digit);
    }
    
    RingNumber result(rules_, negated_digits); 
    return result;
}
