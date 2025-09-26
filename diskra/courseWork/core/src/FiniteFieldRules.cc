#include "FiniteFieldRules.h"
#include <stdexcept>
#include <iostream>

FiniteFieldRules::FiniteFieldRules(const std::string& config_file,
                                   const std::string& variant_name) {
    YAML::Node root = YAML::LoadFile(config_file);
    if (!root["variants"])
        throw std::runtime_error("config missing 'variants'");

    bool found = false;
    for (const auto& type_node : root["variants"]) {
        for (const auto& var_node : type_node.second) {
            if (var_node.first.as<std::string>() == variant_name) {
                init(var_node.second);
                found = true;
                break;
            }
        }
        if (found) break;
    }
    if (!found)
        throw std::runtime_error("variant not found: " + variant_name);
}

void FiniteFieldRules::init(const YAML::Node& variant_node) {
    size_ = variant_node["size"].as<int>();
    zero_ = variant_node["zero_element"].as<std::string>()[0];
    one_  = variant_node["one_element"].as<std::string>()[0];

    YAML::Node rpo = variant_node["rule_plus_one"];
    if (!rpo || !rpo.IsSequence())
        throw std::runtime_error("rule_plus_one must be a sequence");

    std::vector<char> seq = rpo.as<std::vector<char>>();
    if ((int)seq.size() != size_)
        throw std::runtime_error("rule_plus_one size mismatch");

    // строим отображение "следующий элемент"
    std::map<char,char> next;
    for (int i = 0; i < size_ - 1; ++i) {
        next[seq[i]] = seq[i+1];
    }
    next[seq.back()] = seq.front();

    // обходим цикл начиная с zero_
    values_.clear();
    char_to_val_.clear();
    char cur = zero_;
    for (int i = 0; i < size_; ++i) {
        if (char_to_val_.count(cur)) {
            throw std::runtime_error("cycle repetition before full size");
        }
        values_.push_back(cur);
        char_to_val_[cur] = i;
        cur = next[cur];
    }

    if ((int)values_.size() != size_) {
        throw std::runtime_error("incomplete mapping, got " +
            std::to_string(values_.size()) + ", expected " + std::to_string(size_));
    }

    if (values_[1] != one_) {
        std::cerr << "[Warning] one_element != zero_element+1" << std::endl;
    }
}

int FiniteFieldRules::getCharValue(char c) const {
    auto it = char_to_val_.find(c);
    if (it == char_to_val_.end())
        throw std::runtime_error("invalid character: " + std::string(1, c));
    return it->second;
}
