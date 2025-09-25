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

private:
    void initialize_rules(const YAML::Node& variant_node);

    int size_;
    char zero_element_;
    char one_element_;
    std::vector<char> values_;
    std::map<char, int> char_to_val_;
    std::map<int, char> val_to_char_;
};
