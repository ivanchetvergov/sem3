// FiniteFieldRules.cc
#include "FiniteFieldRules.h"

FiniteFieldRules::FiniteFieldRules(const std::string& config_file, const std::string& variant_name) {
    try {
        YAML::Node config = YAML::LoadFile(config_file);
        
        YAML::Node variant_node;
        bool found = false;
        
        for (const auto& variant_type : config["variants"]) {
            if (variant_type.second[variant_name]) {
                variant_node = variant_type.second[variant_name];
                found = true;
                break;
            }
        }
        
        if (!found) {
            throw std::runtime_error("Variant '" + variant_name + "' not found in config.");
        }

        initialize_rules(variant_node);

    } catch (const YAML::BadFile& e) {
        std::cerr << "Error: Config file '" << config_file << "' not found. " << e.what() << std::endl;
        throw;
    } catch (const YAML::Exception& e) {
        std::cerr << "Error parsing YAML: " << e.what() << std::endl;
        throw;
    }
}

void FiniteFieldRules::initialize_rules(const YAML::Node& variant_node) {
    // читаем размер поля и базовые элементы
    size_ = variant_node["size"].as<int>();
    zero_element_ = variant_node["zero_element"].as<char>();
    one_element_ = variant_node["one_element"].as<char>();

    YAML::Node plus_one_node = variant_node["rule_plus_one"];

    // ==========================
    // 1. загружаем правило "+1"
    // ==========================
    if (plus_one_node.IsSequence()) {
        // случай Z11 (правило задано как последовательность)
        std::vector<char> temp_plus_one = plus_one_node.as<std::vector<char>>();

        // строим однозначный цикл: elem[i] -> elem[i+1]
        for (size_t i = 0; i < temp_plus_one.size() - 1; ++i) {
            rule_plus_one_[temp_plus_one[i]] = {temp_plus_one[i + 1]};
        }
        // замыкаем цикл: последний указывает на первый
        rule_plus_one_[temp_plus_one.back()] = {temp_plus_one.front()};
    } else {
        // случай Z8/X1 (правило задано как словарь)
        for (YAML::const_iterator it = plus_one_node.begin(); it != plus_one_node.end(); ++it) {
            char key = it->first.as<char>();
            YAML::Node value_node = it->second;

            // поддержка множественных переходов: f: [c, e]
            if (value_node.IsSequence()) {
                rule_plus_one_[key] = value_node.as<std::vector<char>>();
            } else {
                rule_plus_one_[key] = {value_node.as<char>()};
            }
        }
    }

    // =================================================
    // 2. строим отображения "символ <-> число"
    // =================================================
    //
    // идея: для совместимости с тестами нам нужно уметь
    // представлять элементы как числа 0..size-1.
    // для этого строим цепочку, начиная с zero_element.
    //
    //    если элемент имеет несколько переходов, берём первый.
    //    если в rule_plus_one нет правила для текущего символа —
    //    выходим из цикла (чтобы не падать).
    //
    char current_char = zero_element_;
    for (int i = 0; i < size_; ++i) {
        char_to_val_[current_char] = i;
        val_to_char_[i] = current_char;

        if (!rule_plus_one_.count(current_char)) {
            // если нет следующего — обрываемся
            break;
        }

        // берём первый из возможных переходов
        current_char = rule_plus_one_.at(current_char).front();
    }

    // =================================================
    // 3. валидация 
    // =================================================
    if ((int)char_to_val_.size() < size_) {
        std::cerr << "[warning] variant has only "
                  << char_to_val_.size()
                  << " elements mapped, expected " << size_
                  << ". this may indicate non-standard structure (X-variant)." 
                  << std::endl;
    }
}


int FiniteFieldRules::getSize() const {
    return size_;
}

char FiniteFieldRules::getZeroElement() const {
    return zero_element_;
}

char FiniteFieldRules::getOneElement() const {
    return one_element_;
}

const std::vector<char>& FiniteFieldRules::getNextChar(char current_char) const {
    return rule_plus_one_.at(current_char);
}

int FiniteFieldRules::getCharValue(char character) const {
    return char_to_val_.at(character);
}

char FiniteFieldRules::getValueChar(int value) const {
    return val_to_char_.at(value);
}


