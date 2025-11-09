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
    return add(a, negate(b));
}

RingNumber BigRingArithmetic::subtractPositional(const RingNumber& a, const RingNumber& b) const {
    const char zero = rules_.getZeroElement();
    const char one = rules_.getOneElement();
    
    size_t max_len = std::max(a.length(), b.length());
    std::vector<char> result_digits;
    result_digits.reserve(max_len);
    
    char borrow = zero; // заём
    
    for (size_t i = 0; i < max_len; ++i) {
        char digit_a = (i < a.length()) ? a[i] : zero;
        char digit_b = (i < b.length()) ? b[i] : zero;
        
        // * --- 1 вычитаем заём из digit_a
        if (borrow == one) {
            digit_a = small_.subtract(digit_a, one);
        }
        
        // * --- 2 проверяем: нужен ли новый заём?
        bool need_borrow = isLessThan(digit_a, digit_b);
        
        // * --- 3 вычисляем разность
        char diff;
        if (need_borrow) {
            // занимаем основание: добавляем N к digit_a
            // N = полный круг кольца 
            char increased_digit = digit_a;
            char step = zero;
            
            do {
                increased_digit = small_.plusOne(increased_digit);
                step = small_.plusOne(step);
            } while (step != zero);
            
            // теперь вычитаем
            diff = small_.subtract(increased_digit, digit_b);
            borrow = one;
        } else {
            // обычное вычитание
            diff = small_.subtract(digit_a, digit_b);
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

bool isLessThan(char a, char b, const SmallRingArithmetic& small, const FiniteRingRules& rules) {
    const char zero = rules.getZeroElement();
    
    if (a == b) return false;
    
    // идем от a, считая шаги до b
    char current = a;
    char counter = zero;
    
    while (current != b && current != zero) {
        current = small.plusOne(current);
        counter = small.plusOne(counter);
        
        // если вернулись к a — значит b недостижим, т.е. a > b
        if (current == a) return false;
    }
    
    // если дошли до b — значит a < b
    return (current == b);
}

bool isGreaterOrEqual(const RingNumber& a, const RingNumber& b, 
                      const FiniteRingRules& rules) {
    
    // нормализуем оба числа (удаляем ведущие нули)
    RingNumber a_norm = a;
    RingNumber b_norm = b;
    a_norm.normalize();
    b_norm.normalize();
    
    size_t len_a = a_norm.length();
    size_t len_b = b_norm.length();
                        
    // 1. сравнение по длине (больше цифр = больше число)
    if (len_a != len_b) {
        return len_a > len_b;
    }
    
    // 2. Длины равны → сравниваем поразрядно от СТАРШЕГО к младшему
    for (int i = len_a - 1; i >= 0; --i) {
        char digit_a = a_norm[i];  // старший разряд
        char digit_b = b_norm[i];
        
        int val_a = rules.getCharValue(digit_a);
        int val_b = rules.getCharValue(digit_b);
        
        if (val_a != val_b) {
            return val_a > val_b;
        }
    }
    
    return true;
}

// TODO: реализовать остаток
DivisionResult BigRingArithmetic::divide( const RingNumber& a, const RingNumber& b) const {
    if (b.isZero()) {
        throw std::runtime_error("Division by zero");
    }
    
    const char zero = rules_.getZeroElement();
    const char one = rules_.getOneElement();
    
    RingNumber quotient(rules_);  // Q = 0
    RingNumber remainder = a;     // R = A
    
    RingNumber one_num(rules_, std::string(1, one));
    
    // защита от бесконечного цикла
    const int MAX_ITERATIONS = 10000;
    int iteration = 0;
    
    // пока R >= B
    while (isGreaterOrEqual(remainder, b)) {
        if (++iteration > MAX_ITERATIONS) {
            throw std::runtime_error(
                "Division exceeded max iterations. "
                "R=" + remainder.toString() + 
                ", B=" + b.toString()
            );
        }
        
        // КРИТИЧНО: используем позиционное вычитание, а не кольцевое!
        remainder = subtractPositional(remainder, b);
        
        // Q = Q + 1 (используем кольцевое сложение)
        quotient = add(quotient, one_num);
    }
    
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
