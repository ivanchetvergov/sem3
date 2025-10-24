// tests/gtest.cc 

#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <functional>

#include "FiniteRing.h" 
#include "FiniteRingRules.h" 

std::string VARIANT_NAME;

// --- ФИКСТУРА ---
class RingAxiomTest : public ::testing::Test {
protected:
    std::unique_ptr<FiniteRingRules> rules_;
    std::unique_ptr<FiniteRing> calculator_;
    std::vector<char> symbols;
    char zero_element;
    char one_element;

    void SetUp() override {
        const std::string config_path = "../config.yaml"; 
        const std::string& variant_to_use = VARIANT_NAME.empty() ? "variant_1" : VARIANT_NAME;

        rules_ = std::make_unique<FiniteRingRules>(config_path, variant_to_use);
        calculator_ = std::make_unique<FiniteRing>(*rules_);

        for (int i = 0; i < rules_->getSize(); ++i)
            symbols.push_back(rules_->getValueChar(i));

        zero_element = rules_->getZeroElement();
        one_element = rules_->getOneElement();
    }
    
    bool hasInverse(char val) {
        try {
            calculator_->findMultiplicativeInverse(val);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    void run_ternary_test(std::function<void(char, char, char)> test_func, const std::string& description) {
        int check_count = 0;
        for (char a : symbols) {
            for (char b : symbols) {
                for (char c : symbols) {
                    test_func(a, b, c);
                    check_count++;
                }
            }
        }
        std::cout << "  > " << description << " passed. Total checks: " << check_count << std::endl;
    }
};


TEST_F(RingAxiomTest, Axiom_01_Commutativity) {
    for (char a : symbols) {
        for (char b : symbols) {
            //* a + b == b + a
            ASSERT_EQ(calculator_->add(a, b), calculator_->add(b, a))
                << "Add failed commutativity for: " << a << " + " << b;

            //* a * b == b * a
            ASSERT_EQ(calculator_->multiply(a, b), calculator_->multiply(b, a))
                << "Mul failed commutativity for: " << a << " * " << b;
        }
    }
}


TEST_F(RingAxiomTest, Axiom_02_IdentityElements) {
    const char zero = zero_element;
    const char one = one_element;

    for (char x : symbols) {
        //*  x + 0 == x
        ASSERT_EQ(calculator_->add(x, zero), x)
            << "Additive identity failed: " << x << " + " << zero << " != " << x;

        //* x * 1 == x
        ASSERT_EQ(calculator_->multiply(x, one), x)
            << "Multiplicative identity failed: " << x << " * " << one << " != " << x;
        
        //* x * 0 == 0
        ASSERT_EQ(calculator_->multiply(x, zero), zero)
            << "Zero absorption failed: " << x << " * " << zero << " != " << zero;
    }
}


TEST_F(RingAxiomTest, Axiom_03_Associativity) {
    run_ternary_test([this](char a, char b, char c) {
        //* (a + b) + c == a + (b + c)
        char left_add = calculator_->add(calculator_->add(a, b), c);
        char right_add = calculator_->add(a, calculator_->add(b, c));
        ASSERT_EQ(left_add, right_add)
            << "Add failed associativity for: (" << a << "+" << b << ")+" << c;
        
        //* (a * b) * c == a * (b * c)
        char left_mul = calculator_->multiply(calculator_->multiply(a, b), c);
        char right_mul = calculator_->multiply(a, calculator_->multiply(b, c));
        ASSERT_EQ(left_mul, right_mul)
            << "Mul failed associativity for: (" << a << "*" << b << ")*" << c;
    }, "Associativity (N^3)");
}


TEST_F(RingAxiomTest, Axiom_04_Distributivity) {
    run_ternary_test([this](char a, char b, char c) {
        //* a * (b + c) == a * b + a * c
        char left = calculator_->multiply(a, calculator_->add(b, c));
        char right = calculator_->add(calculator_->multiply(a, b), calculator_->multiply(a, c));
        
        ASSERT_EQ(left, right)
            << "Distributivity failed for: " << a << " * (" << b << " + " << c << ")";
    }, "Distributivity (N^3)");
}


TEST_F(RingAxiomTest, Axiom_05_InverseAndClosure) {
    const char zero = zero_element;

    for (char a : symbols) {
        for (char b : symbols) {
            //* a - b = c (замкнутность)
            ASSERT_NO_THROW(calculator_->subtract(a, b))
                << "Subtraction failed for: " << a << " - " << b;

            //*  a * 1/a != 1 
            if (b == zero || !hasInverse(b)) {
                ASSERT_THROW(calculator_->divide(a, b), std::exception)
                    << "Division expected throw for: " << a << " / " << b;
            } else {
                //* a * 1/a == 1 
                char result = calculator_->divide(a, b);
                ASSERT_EQ(calculator_->multiply(result, b), a)
                    << "Division property (a/b)*b != a failed for: " << a << " / " << b;
            }
        }
    }
}



int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    if (argc >= 2) {
        VARIANT_NAME = argv[1]; 
    } else {
        std::cerr << "WARNING: Variant name not provided. Using default 'variant_1'." << std::endl;
        VARIANT_NAME = "variant_1"; 
    }
    
    return RUN_ALL_TESTS();
}