class RingNumber {
public:
    RingNumber(const FiniteRingRules& rules);
    RingNumber(const FiniteRingRules& rules, const string& value);
    RingNumber(const FiniteRingRules& rules, const vector<char>& digits);
    
    // Доступ к цифрам
    size_t length() const;
    char operator[](size_t index) const;
    char& operator[](size_t index);
    
    // Преобразования
    string toString() const;
    vector<char> toVector() const;
    
    // Нормализация (удаление ведущих нулей)
    void normalize();
    
    // Проверки
    bool isZero() const;
    bool isValid() const;
    
private:
    const FiniteRingRules& rules_;
    vector<char> digits_;  // Младший разряд первый!
    
    void validate();
};