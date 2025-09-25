// core/include/FiniteFieldRules.h
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "yaml-cpp/yaml.h"

class FiniteFieldRules {
public:

    FiniteFieldRules(const std::string& config_file, const std::string& variant_name);

    int getSize() const;
    char getZeroElement() const;
    char getOneElement() const;

    char getValueChar(int value) const;
    int getCharValue(char value) const;
    
    const std::vector<char>& getNextChar(char current_char) const; 

private:
    int size_; 
    char zero_element_; 
    char one_element_;

    std::map<char, int> char_to_val_;
    std::map<int, char> val_to_char_;

    std::map<char, std::vector<char>> rule_plus_one_;

    void initialize_rules(const YAML::Node& variant_node);
};