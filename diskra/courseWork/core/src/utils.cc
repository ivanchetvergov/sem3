// core/src/utils.cc
#include "BigRingArithmetic.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

bool BigRingArithmetic::isLessThan(char a, char b) const {
    const char zero = rules_.getZeroElement();
    
    if (a == b) return false;
    
    // идем от a, считая шаги до b
    char current = a;
    char counter = zero;
    
    while (current != b && current != zero) {
        current = small_.plusOne(current);
        counter = small_.plusOne(counter);
        
        // если вернулись к a — значит b недостижим, т.е. a > b
        if (current == a) return false;
    }
    
    // если дошли до b — значит a < b
    return (current == b);
}

bool BigRingArithmetic::isGreaterOrEqual(const RingNumber& a, const RingNumber& b) const {
    
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
        
        int val_a = rules_.getCharValue(digit_a);
        int val_b = rules_.getCharValue(digit_b);
        
        if (val_a != val_b) {
            return val_a > val_b;
        }
    }
    
    return true;
}
