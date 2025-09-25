#include "FiniteFieldRules.h"
#include <yaml-cpp/yaml.h>
#include <stdexcept>
#include <iostream>

FiniteFieldRules::FiniteFieldRules(const std::string& config_file, const std::string& variant_name) {
    YAML::Node root = YAML::LoadFile(config_file);
    if (!root["variants"]) throw std::runtime_error("variants not found in config");

    bool found = false;
    for (const auto& type_node : root["variants"]) {
        for (const auto& var_node : type_node.second) {
            if (var_node.first.as<std::string>() == variant_name) {
                initialize_rules(var_node.second);
                found = true;
                break;
            }
        }
        if (found) break;
    }
    if (!found) throw std::runtime_error("variant not found: " + variant_name);
}

void FiniteFieldRules::initialize_rules(const YAML::Node& variant_node) {
    size_ = variant_node["size"].as<int>();
    zero_element_ = variant_node["zero_element"].as<std::string>()[0];
    one_element_ = variant_node["one_element"].as<std::string>()[0];

    values_ = variant_node["symbols"].as<std::vector<char>>();

    if (values_.size() != size_) {
        throw std::runtime_error("symbols size does not match field size");
    }

    char_to_val_.clear();
    val_to_char_.clear();
    for (int i = 0; i < size_; ++i) {
        char_to_val_[values_[i]] = i;
        val_to_char_[i] = values_[i];
    }
}

int FiniteFieldRules::getSize() const { return size_; }
char FiniteFieldRules::getZeroElement() const { return zero_element_; }
char FiniteFieldRules::getOneElement() const { return one_element_; }

char FiniteFieldRules::getValueChar(int value) const {
    auto it = val_to_char_.find(value);
    if (it == val_to_char_.end()) throw std::runtime_error("invalid value index");
    return it->second;
}

int FiniteFieldRules::getCharValue(char value) const {
    auto it = char_to_val_.find(value);
    if (it == char_to_val_.end()) throw std::runtime_error("invalid character");
    return it->second;
}
