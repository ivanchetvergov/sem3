#include "FiniteField.h"
#include <stdexcept>

char FiniteField::add(char a, char b) const {
    return rules_.getValueChar(
        (rules_.getCharValue(a) + rules_.getCharValue(b)) % rules_.getSize()
    );
}

char FiniteField::subtract(char a, char b) const {
    int n = rules_.getSize();
    return rules_.getValueChar(
        (rules_.getCharValue(a) - rules_.getCharValue(b) + n) % n
    );
}

char FiniteField::multiply(char a, char b) const {
    return rules_.getValueChar(
        (rules_.getCharValue(a) * rules_.getCharValue(b)) % rules_.getSize()
    );
}

char FiniteField::divide(char a, char b) const {
    int n = rules_.getSize();
    int vb = rules_.getCharValue(b);
    if (vb == 0)
        throw std::runtime_error("ivision by zero");

    // ищем мультипликативный обратный
    int inv = -1;
    for (int i = 0; i < n; ++i) {
        if ((vb * i) % n == 1) {
            inv = i;
            break;
        }
    }
    if (inv == -1)
        throw std::runtime_error("no multiplicative inverse for: " + std::string(1, b));

    return rules_.getValueChar((rules_.getCharValue(a) * inv) % n);
}

char FiniteField::findMultiplicativeInverse(char element) const {
    const char one = rules_.getOneElement();
    const int size = rules_.getSize();

    int target_index = rules_.getCharValue(element);

    for (int i = 0; i < size; ++i) {
        char current_char = rules_.getValueChar(i);
        

        char product = multiply(element, current_char); 
        
        if (product == one) {
            return current_char; 
        }
    }
    throw std::runtime_error("Multiplicative inverse does not exist for the element.");
}
