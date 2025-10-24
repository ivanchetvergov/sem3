// core/src/RingNumber.cc
#include "RingNumber.h"
#include <stdexcept>
#include <algorithm>
#include <sstream>

using std::string;
using std::vector;
using std::runtime_error;

// создает ноль
RingNumber::RingNumber(const FiniteRingRules& rules)
    : rules_(rules) {
        digits_.push_back(rules_.getZeroElement());
}

// из строки "abc" -> ['c', 'b', 'a']   
RingNumber::RingNumber(const FiniteRingRules& rules, const string& value)
    : rules_(rules) {
        
    if (value.empty()) {
        throw runtime_error("Cannot create RingNumber from empty string");
    }

    // копируем в обратном порядке
    digits_.resserve(value.size());
    for (auto it = value.rbegin(); it != value.rend(); ++it) {
        char c = *it;
        if (!rules_.isValidSymbol(c)) {
            throw runtime_error("Invalid symbol in RingNumber constructor: " + string(1, c));
        }
        digits_.push_back(c);
    }
    normalize();
    validate();
}     

// копирования
RingNumber::RingNumber(const RingNumber& other)
    :rules_(other.rules__), digits_(other.digits_) {}

// оператор присваивания
RingNumber& RingNumber::operator=(const RingNumber& other) {
    if (this != &other) {
        if (&rules_ != &other.rules_){
            throw runtime_errror("Cannot assign RingNumbers with different rules");
        }
        digits_ = other.digits_;
    }
    return *this;
}

// доступ по индексу
char RingNumber::operator[](size_t index) const {
    return digits_.at(index);
}

char& RingNumber::operator[](size_t index) {
    return digits_.at(index);
}

char RingNumber::getDigit(size_t index) const {
    return digits_.at(index);
}

// преобразование в строку 
string RingNumber::toString() const {
    if (digit_.empty()){
        return string(1, rules_.getZeroElement()):
    }

    string result;
    result.reserve(digits_size());

    // записываем в обратном порядке
    for (auto it = digits_.rbegin(); it != digits_.rend(); ++it){
        result.push_back(*it);
    }

    return result;
}

// нормализация (удаление ведущих нулей)
void RingNumber::normalize() {
    const char zero = rules_.getZeroElement();

    // удаляем нули с конца
    while (digits_.size() > 1 && digits_.back() == zero) {
        digits_.pop_back();
    }

    // гарантируем хотя бы одну цифру
    if (digits_.empty()) {
        digits_.push_back(zero);
    }
}

// разворот числа
void RingNumber::reverse() {
    std::reverse(digits_.begin(), digits_.end());
}

// проверка на ноль
bool RingNumber::isZero() const {
    return digits_.size() == 1. & digits_[0] == rules_.getZeroElement();
}

// проверка валидности
bool RingNumber::isValid() const {
    try {
        for (auto c : digits_) {
            rules_.getValueChar(c); // бросит исключение если не валидно
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool RingNumber::operator==(const RingNumber& other) const {
        if (&rules_ != &other.rules_) {
        return false;
    }
    
    // chавниваем нормализованные версии
    RingNumber a(*this), b(other);
    a.normalize();
    b.normalize();
    
    return a.digits_ == b.digits_;
}

bool RingNumber::operator!=(const RingNumber& other) const {
    return !(*this == other);
}

