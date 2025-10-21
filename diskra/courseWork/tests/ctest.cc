#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <functional>
#include "FiniteFieldRules.h"
#include "FiniteField.h"

void run_test(const std::string& description, std::function<void()> test_func) {
    try {
        test_func();
        std::cout << "SUCCESS: " << description << " passed." << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FAILURE: " << description << " failed. " << e.what() << std::endl;
        exit(1); 
    }
}

void run_all_tests(const FiniteField& calculator) {
    run_test("Addition", [&]() {
        char result = calculator.add(
                                      calculator.getRules().getOneElement(), 
                                      calculator.getRules().getZeroElement()
                                    );
        if (result != calculator.getRules().getOneElement()) {
            throw std::runtime_error("Addition with zero element failed.");
        }
    });

    run_test("Subtraction", [&]() {
        char result = calculator.subtract(
                                          calculator.getRules().getOneElement(), 
                                          calculator.getRules().getOneElement()
                                         );
        if (result != calculator.getRules().getZeroElement()) {
            throw std::runtime_error("Subtraction of a number from itself failed.");
        }
    });

    run_test("Multiplication", [&]() {
        char result = calculator.multiply(
                                          calculator.getRules().getOneElement(), 
                                          calculator.getRules().getZeroElement()
                                        );
        if (result != calculator.getRules().getZeroElement()) {
            throw std::runtime_error("Multiplication with zero element failed.");
        }
    });

    run_test("Division", [&]() {
        char result = calculator.divide(
                                          calculator.getRules().getOneElement(), 
                                          calculator.getRules().getOneElement()
                                        );
        if (result != calculator.getRules().getOneElement()) {
            throw std::runtime_error("Division of a number by itself failed.");
        }
    });

    run_test("Division by zero", [&]() {
        try {
            calculator.divide(
                                calculator.getRules().getOneElement(), 
                                calculator.getRules().getZeroElement()
                            );
            throw std::runtime_error("Expected an exception for division by zero, but none was thrown.");
        } catch (const std::runtime_error& e) {
            if (std::string(e.what()).find("division by zero") == std::string::npos) {
                throw;
            }
        }
    });
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <variant_name>" << std::endl;
        return 1;
    }

    std::string variant_name = argv[1];
    
    try {
        FiniteFieldRules rules("../config.yaml", variant_name);
        FiniteField calculator(rules);

        std::cout << "--- Starting tests for variant: " << variant_name << " ---" << std::endl;
        run_all_tests(calculator);
        std::cout << "--- All tests for variant " << variant_name << " passed. ---" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error during test run for " << variant_name << ": " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}