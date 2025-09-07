// Core.cc
#include "Core.h"

#include <random> 
#include <iostream>

std::string toBinary(int number, int length) {
    if (number < 0) {
        return "";
    }
    std::string binaryString = "";
    for (int i = length - 1; i >= 0; --i) {
        binaryString += ((number >> i) & 1) ? '1' : '0';
    }
    return binaryString;
}

std::vector<std::string> generateGrayCode(int n) {
    if (n < 0)  return {};  

    if (n == 0) return {""};

    std::vector<std::string> grayCodes;
    int totalCodes = 1 << n; // 2^n
    
    for (int i = 0; i < totalCodes; ++i) {
        int gray_i = i ^ (i >> 1);
        grayCodes.push_back(toBinary(gray_i, n));
    }
    
    return grayCodes;
}

