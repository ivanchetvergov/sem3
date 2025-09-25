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
    size_ = variant_node["size"].as<int>();

    // считываем аддитивную и мультипликативную единицы
    zero_element_ = variant_node["zero_element"].as<char>();
    one_element_ = variant_node["one_element"].as<char>();

    // считываем правило "+1" и заполняем универсальный map
    YAML::Node plus_one_node = variant_node["rule_plus_one"];
    
    // перебираем все пары ключ-значение в правиле "+1"
    for (YAML::const_iterator it = plus_one_node.begin(); it != plus_one_node.end(); ++it) {
        char key = it->first.as<char>();
        YAML::Node value_node = it->second;

        // значение является списком (как в варианте X1 ), загружаем его в вектор
        if (value_node.IsSequence()) {
            std::vector<char> values = value_node.as<std::vector<char>>();
            rule_plus_one_[key] = values;
        } else { // если это одиночный символ (как в Z8 )
            char value = value_node.as<char>();
            rule_plus_one_[key] = {value}; // помещаем его в вектор из одного элемента
        }
    }

    std::vector<char> temp_plus_one;
    temp_plus_one = variant_node["rule_plus_one"].as<std::vector<char>>();

    // создаем отображения символ <-> число
    // (этот цикл теперь будет обрабатывать все варианты, включая Z8 [cite: 6] и Z11 [cite: 9])
    for (int i = 0; i < size_; ++i) {
        char_to_val_[temp_plus_one[i]] = i;
        val_to_char_[i] = temp_plus_one[i];
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


