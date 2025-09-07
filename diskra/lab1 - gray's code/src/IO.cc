// IO.cc
#include "IO.h"
#include "Exceptions.h"
#include <limits>
#include <iostream>
#include <string>

int readInteger(const std::string& data) {
    int value;
    std::cout << data;
    std::cin >> value;

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw InvalidValueException("некорректный ввод. ожидалось число.");
    }

    char nextChar = std::cin.peek();
    if (nextChar != EOF && nextChar != '\n') {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw InvalidValueException("лишние символы после числа.");
    }

    return value;
}