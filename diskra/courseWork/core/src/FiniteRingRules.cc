// core/src/FiniteRingRules.cc
#include "FiniteRingRules.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

using std::string;
using std::vector;
using std::map;
using std::runtime_error;
using std::cerr;
using std::endl;

FiniteRingRules::FiniteRingRules(const string& config_file,
                                   const string& variant_name) {
    YAML::Node root;
    
    try {
        root = YAML::LoadFile(config_file);
    } catch (const YAML::Exception& e) {
        throw runtime_error("Failed to load config file '" + config_file + "': " + e.what());
    }
    
    if (!root["variants"]) {
        throw runtime_error("Config missing 'variants' section");
    }
    
    bool found = false;
    
    // * ищем вариант в иерархической структуре
    for (const auto& type_node : root["variants"]) {
        for (const auto& var_node : type_node.second) {
            if (var_node.first.as<string>() == variant_name) {
                init(var_node.second); // вызываем инициализацию
                found = true;
                break;
            }
        }
        if (found) break;
    }
    
    if (!found) {
        throw runtime_error("Variant not found: '" + variant_name + "'");
    }
}

void FiniteRingRules::init(const YAML::Node& variant_node) {
    // * 1 --- чтение базовых параметров
    if (!variant_node["size"] || !variant_node["zero_element"] || !variant_node["one_element"]) {
        throw runtime_error("Variant must contain: size, zero_element, one_element");
    }
    
    size_ = variant_node["size"].as<int>();    
    string zero_str = variant_node["zero_element"].as<string>();
    string one_str = variant_node["one_element"].as<string>();
    
    if (zero_str.empty() || one_str.empty()) {
        throw runtime_error("Zero and one elements cannot be empty");
    }
    
    zero_ = zero_str[0];
    one_ = one_str[0];
    
    if (zero_ == one_) {
        throw runtime_error("Zero and one elements must be different");
    }
    
    // * 2 --- чтение правила "плюс один"
    YAML::Node rpo = variant_node["rule_plus_one"];
    
    if (!rpo || !rpo.IsSequence()) {
        throw runtime_error("rule_plus_one must be a sequence");
    }
    
    vector<string> seq_strs = rpo.as<vector<string>>();
    // vector<char> seq_strs = rpo.as<vector<char>>();

    if ((int)seq_strs.size() != size_) {
        throw runtime_error("rule_plus_one size must match ring size");
    }
    
    // * 3 --- формирование последовательности
    vector<char> seq;
    seq.reserve(size_);
    
    for (const auto& s : seq_strs) {
        if (s.empty()) {
            throw runtime_error("Empty string in rule_plus_one");
        }
        seq.push_back(s[0]); // берем первый символ (char)
        // if (!s){
        //     throw runtime_error("Invalid character in rule_plus_one");
        // }
        // seq.push_back(s);
    }
    
    // * 4 --- ищем zero_el в послед
    int zero_position = -1;
    for (int i = 0; i < size_; ++i) {
        if (seq[i] == zero_) {
            zero_position = i;
            break;
        }
    }
    if (zero_position == -1) {
        throw runtime_error("Zero element not found in rule_plus_one sequence");
    }

    // * 5 --- послед начиная с нуля (задаем порядок X -> X + 1)
    
    // итоговоые контейнеры класса 
    values_.clear();        // контейнер значений
    char_to_val_.clear();   // мапа char -> int
    
    for (int i = 0; i < size_; ++i){
        int idx = (zero_position + i) % size_;  // считаем индекс в ориг последов.
        char cur = seq[idx];                    // текущий символ

        values_.push_back(cur);
        char_to_val_[cur] = i;
    }
}

#ifdef DEBUG
    cerr << "\n--- Ring initialized ---" << endl;
    cerr << "Size: " << size_ << endl;
    cerr << "Zero: " << zero_ << " (index 0)" << endl;
    cerr << "One:  " << one_ << " (index 1)" << endl;
    cerr << "Order: ";
    for (int i = 0; i < size_; ++i) {
        cerr << values_[i];
        if (i < size_ - 1) cerr << " -> ";
    }
    cerr << " -> " << values_[0] << " (cycle)" << endl;
    cerr << "---------------------=\n" << endl;
#endif


int FiniteRingRules::getCharValue(char c) const {
    auto it = char_to_val_.find(c);
    
    if (it == char_to_val_.end()) {
        throw runtime_error("Invalid character: '" + string(1, c) + 
                          "' (not in ring)");
    }
    
    return it->second;
}

bool FiniteRingRules::isValidChar(char c) const {
    return char_to_val_.find(c) != char_to_val_.end();
}