// core/src/SmallRingArithmetic.cc
#include "SmallRingArithmetic.h"
#include <stdexcept>

char SmallRingArithmetic::add(char a, char b) const {
    // ? вариант через сложение индексов O(1)
    // получаем индексы символов
    int va = rules_.getCharValue(a);
    int vb = rules_.getCharValue(b);
    int n = rules_.getSize();
    
    // Складываем индексы по модулю
    int result_index = (va + vb) % n;
    
    return rules_.getValueChar(result_index);

    // // ! вариант через последовательное + 1 O(N)
    // const char zero = rules_.getZeroElement();
    // const int vb = rules_.getCharValue(b); // сколько раз нужно прибавить 1
    
    // if (b == zero) {
    //     return a;
    // }
    
    // char result = a;
    
    // for (int i = 0; i < vb; ++i) {
    //     int current_val = rules_.getCharValue(result);
    //     int next_val = (current_val + 1) % rules_.getSize(); 
    //     result = rules_.getValueChar(next_val);
    // }
    
    // return result;
}

char SmallRingArithmetic::findAdditiveInverse(char element) const {
    const char zero = rules_.getZeroElement();
    
    // ноль обратен сам себе
    if (element == zero) {
        return zero;
    }
    
    // ием x такой что add(element, x) = zero
    const int size = rules_.getSize();
    
    for (int i = 0; i < size; ++i) {
        char candidate = rules_.getValueChar(i);
        if (add(element, candidate) == zero) { 
            return candidate;
        }
    }
    
    throw std::runtime_error("Additive inverse not found for element: " + 
                           std::string(1, element));
}

char SmallRingArithmetic::subtract(char a, char b) const {
    // ? вариант разницы индексов O(1)
    // int va = rules_.getCharValue(a);
    // int vb = rules_.getCharValue(b);
    // int n = rules_.getSize();
    
    // // Вычитаем с учетом модуля
    // int result_index = (va - vb + n) % n;
    
    // return rules_.getValueChar(result_index);

    // ! вариант через сложение с обратным (N))
    char neg_b = findAdditiveInverse(b); 
    return add(a, neg_b);
}

char SmallRingArithmetic::multiply(char a, char b) const {
    
    // ? умножаем индексы по модулю N (O(1) операция)
    // long long product_val = (long long)va * vb;
    // int result_index = product_val % n;
    
    // return rules_.getValueChar(result_index);

    // ! умножение через повторное сложение O(N^2)
    const char zero = rules_.getZeroElement();
    
    // умножение на ноль
    if (a == zero || b == zero) {
        return zero;
    }
    // умножение на единицу
    if (b == rules_.getOneElement()) {
        return a;
    }
    if (a == rules_.getOneElement()) {
        return b;
    }
    
    // получаем индекс второго операнда
    int vb = rules_.getCharValue(b);
    
    char result = zero;
    for (int i = 0; i < vb; ++i) {
        result = add(result, a);
    }
    
    return result;
}

char SmallRingArithmetic::divide(char a, char b) const {
    const char zero = rules_.getZeroElement();
    
    // деление на ноль
    if (b == zero) {
        throw std::runtime_error("Division by zero");
    }
    
    // находим обратный элемент
    char inv = findMultiplicativeInverse(b);
    return multiply(a, inv);
}

char SmallRingArithmetic::findMultiplicativeInverse(char element) const {
    const char zero = rules_.getZeroElement();
    const char one = rules_.getOneElement();
    
    // У нуля нет обратного
    if (element == zero) {
        throw std::runtime_error("Zero has no multiplicative inverse");
    }
    
    const int size = rules_.getSize();
    
    // Ищем x такой, что element * x = 1
    for (int i = 0; i < size; ++i) {
        char candidate = rules_.getValueChar(i);
        char product = multiply(element, candidate);
        
        if (product == one) {
            return candidate;
        }
    }
    
    // Не нашли обратный
    throw std::runtime_error("No multiplicative inverse for element: " + 
                           std::string(1, element));
}