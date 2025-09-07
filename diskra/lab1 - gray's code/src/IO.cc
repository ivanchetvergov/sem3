// IO.cc
#include "IO.h"
#include <limits>
#include <iostream>

int readInteger(const std::string& data) {
    int value;
    while (true) {
        std::cout << data;
        std::cin >> value;

        if (std::cin.fail()) {
            std::cout << "ошибка! введите число заново.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // очистка бувера
            continue; 
        }

        char nextChar = std::cin.peek();
        if (nextChar != EOF && nextChar != '\n') {
            std::cout << "ошибка! введите лишь число и ничео лишнего.\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue; 
        }

        break;
    }
    return value;
}
