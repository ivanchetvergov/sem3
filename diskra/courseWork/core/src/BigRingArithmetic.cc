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
// ' для переноса
    
    size_t max_len = std::max(a.length(), b.length());
    
    std::vector<char> result_digits;
    result_digits.reserve(max_len + 1);
    
    char carry_out = zero_; // перенос
    
    // * складываем поразрядно от младших к старшим
    for (size_t i = 0; i < max_len || carry_out != zero_; ++i) {
        char digit_a = a.getDigit(i);
        char digit_b = b.getDigit(i);
        char carry_in = carry_out;
        
        // --- 1. сложение A + B ---
        char sum_mid = small_.add(digit_a, digit_b);
        
        // --- 2. проверка C1: Перенос от A + B ---
        char carry_1 = zero_;
        // C1 = 1, если V(A) + V(B) >= N. Это эквивалентно (A+B) mod N < A.
        if (isLessThan(sum_mid, digit_a)) {
            carry_1 = one_;
        }
        
        // --- 3. сложение S_mid + C_in. Получаем финальную сумму S_final ---
        char sum_final = small_.add(sum_mid, carry_in);
        
        // --- 4. проверка C2: Перенос от S_mid + C_in ---
        char carry_2 = zero_;
        // C2 = 1, если S_mid + C_in >= N. Это возможно, только если C_in=1
        if (carry_in != zero_) { 
            if (isLessThan(sum_final, sum_mid)) {
                carry_2 = one_;
            }
        }
        
        // --- 5. общий перенос C_out = C1 OR C2 ---
        if (carry_1 != zero_ || carry_2 != zero_) {
            carry_out = one_;
        } else {
            carry_out = zero_;
        }
        
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
        char digit_a = (i < a.length()) ? a[i] : zero_;
        char digit_b = (i < b.length()) ? b[i] : zero_;
        
        // * --- 1 вычитаем заём из digit_a
        if (borrow == one_) {
            digit_a = small_.subtract(digit_a, one_);
        }
        
        // * --- 2 проверяем: нужен ли новый заём?
        bool need_borrow = isLessThan(digit_a, digit_b);
        
        // * --- 3 вычисляем разность
        char diff;
        if (need_borrow) {
            // занимаем основание: добавляем N к digit_a
            char increased_digit = digit_a;
            char step = zero_;
            
            do {
                increased_digit = small_.plusOne(increased_digit);
                step = small_.plusOne(step);
            } while (step != zero_);
            
            diff = small_.subtract(increased_digit, digit_b);
            borrow = one_;
        } else {
            diff = small_.subtract(digit_a, digit_b);
            borrow = zero_;
        }
        
        result_digits.push_back(diff);
    }
    
    RingNumber result(rules_, result_digits);
    result.normalize();
    return result;
}

// * --- УМНОЖЕНИЕ ---
RingNumber BigRingArithmetic::multiply(const RingNumber& a, const RingNumber& b) const {
    const char zero_ = rules_.getZeroElement();
    
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

// * --- ДЕЛЕНИЕ С ОСТАТКОМ (Деление Столбиком) ---
DivisionResult BigRingArithmetic::divide(const RingNumber& a, const RingNumber& b) const {
    if (b.isZero()) {
        throw std::runtime_error("Division by zero_");
    }

    RingNumber quotient(rules_);
    RingNumber remainder = a;

    // * если делимое меньше делителя
    if (b.length() > a.length() || (b.length() == a.length() && isGreaterOrEqual(b, a))) {
        if (!isGreaterOrEqual(a, b)) {
            return DivisionResult(quotient, remainder); 
        }
    }
    
    // * 2. выравнивание (Shift Setup)
    // деление происходит с самой большой возможной степени
    int shift_amount = (int)a.length() - (int)b.length();
    
    // * 3. главный цикл деления
    RingNumber shifted_b = shiftLeft(b, shift_amount);
    
    // итерируемся по разрядам частного, от старшего к младшему
    for (int i = shift_amount; i >= 0; --i) {
        
        // 3.1. находим цифру частного (q_digit)
        char q_digit = rules_.getZeroElement();

        RingNumber temp_remainder = remainder; 
        
        // ищем, сколько раз shifted_b помещается в remainder
        q_digit = findQuotientDigit(remainder, shifted_b);

        // если нашли ненулевую цифру частного
        if (q_digit != rules_.getZeroElement()) {
            
            // 3.2. обновляем частное: Q = Q + (q_digit * 10^i)
            RingNumber q_digit_num(rules_, std::string(1, q_digit));
            RingNumber partial_quotient = shiftLeft(q_digit_num, i);
            quotient = add(quotient, partial_quotient);
            
            // 3.3. обновляем остаток: remainder = remainder - q_digit * shifted_b
            // создаем вычитаемое: q_digit * shifted_b
            RingNumber subtraction_amount = multiplyByDigit(shifted_b, q_digit);
            
            // вычитаем: remainder = remainder - subtraction_amount
            remainder = subtractPositional(remainder, subtraction_amount); 
        }
        
        // 3.4. сдвигаем делитель вправо для следующей итерации
        if (i > 0) {
            shifted_b = shiftRight(shifted_b, 1);
        }
    }

    remainder.normalize(); // убедимся, что остаток чистый
    quotient.normalize();
    
    return DivisionResult(quotient, remainder);
}

// * --- АДДИТИВНАЯ ИНВЕРСИЯ (отрицание) ---
RingNumber BigRingArithmetic::negate(const RingNumber& a) const {
    if (a.isZero()) {
        return a;
    }
    const int Modulus = rules_.getSize();
    
    // --- 1. вычисляем поразрядную инверсию ---
    std::vector<char> inverted_digits;
    inverted_digits.resize(MAX_DIGITS); 
    
    // заполняем все 8 разрядов
    for (size_t i = 0; i < MAX_DIGITS; ++i) {
        // получаем цифру A[i] или 0, если i > a.length()
        char digit = a.getDigit(i); 
        int val = rules_.getCharValue(digit);
        
        // поразрядная инверсия
        int inverted_val = (Modulus - 1) - val; 
        inverted_digits[i] = rules_.getValueChar(inverted_val);
    }
    
    RingNumber inverted_a(rules_, inverted_digits);
    
    // --- 2. добавляем 1 для получения (-A) = (~A) + 1 ---
    const char one_ = rules_.getOneElement();
    RingNumber one_num(rules_, std::string(1, one_));
    
    RingNumber result = add(inverted_a, one_num); 
    result.normalize();
    
    return result; 
}

// * --- ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ---
RingNumber BigRingArithmetic::multiplyByDigit(const RingNumber& num, char digit) const {

    if (digit == zero_ || num.isZero()) {
        return RingNumber(rules_);
    }

    if (digit == one_) {
        return num;
    }

    RingNumber result(rules_);
    char counter = zero_;

    while (counter != digit) {
        result = add(result, num); // add() проверяет переполнение
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
    result.normalize(); 
    return result;
}

char BigRingArithmetic::findQuotientDigit(const RingNumber& remainder, const RingNumber& shifted_divisor) const {

    char q_digit = zero_;
    
    // временный остаток для цикла
    RingNumber temp_remainder = remainder; 
        
    // ищем, сколько раз вычитается shifted_divisor из remainder
    while (isGreaterOrEqual(temp_remainder, shifted_divisor)) {
        // вычитаем и увеличиваем q_digit
        temp_remainder = subtractPositional(temp_remainder, shifted_divisor);
        q_digit = small_.plusOne(q_digit);
    }

    return q_digit;
}