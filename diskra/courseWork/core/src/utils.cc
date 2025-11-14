// core/src/utils.cc
#include "BigRingArithmetic.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

// * --- СРАВНЕНИЕ СИМВОЛОВ: a < b? ---
bool BigRingArithmetic::isLessThan(char a, char b) const {
    
    int val_a = rules_.getCharValue(a);
    int val_b = rules_.getCharValue(b);
    
    return val_a < val_b; 
}

// * --- СРАВНЕНИЕ ЧИСЕЛ: a >= b? ---
bool BigRingArithmetic::isGreaterOrEqual(const RingNumber& a, const RingNumber& b) const {
    
    // нормализуем оба числа
    RingNumber a_norm = a;
    RingNumber b_norm = b;
    a_norm.normalize();
    b_norm.normalize();
    
    size_t len_a = a_norm.length();
    size_t len_b = b_norm.length();
                        
    // 1. сравнение по длине
    if (len_a != len_b) {
        return len_a > len_b;
    }
    
    // 2. сравниваем поразрядно от СТАРШЕГО к младшему
    for (int i = len_a - 1; i >= 0; --i) {
        char digit_a = a_norm[i];
        char digit_b = b_norm[i];
        
        int val_a = rules_.getCharValue(digit_a);
        int val_b = rules_.getCharValue(digit_b);
        
        if (val_a != val_b) {
            return val_a > val_b;
        }
    }
    
    return true; // числа равны
}