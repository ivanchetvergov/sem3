#include "FiniteFieldRules.h"
#include <stdexcept>
#include <iostream>

using std::string;
using std::vector;
using std::map;
using std::runtime_error;
using std::cerr;
using std::endl;

// * конструктор из файла конфигурации и имени варианта
FiniteFieldRules::FiniteFieldRules(const string& config_file,
                                   const string& variant_name) {
    // загружаем YAML
    YAML::Node root = YAML::LoadFile(config_file);
    if (!root["variants"])
        throw runtime_error("config missing 'variants'");
    
    bool found = false;
    // ищем нужный вариант
    for (const auto& type_node : root["variants"]) {
        // перебираем варианты внутри типа
        for (const auto& var_node : type_node.second) {
            // * если нашли нужный вариант - инициализируемся
            if (var_node.first.as<string>() == variant_name) {
                // вызываем инициализацию правил
                init(var_node.second);
                found = true;
                break;
            }
        }
        if (found) break;
    }
    if (!found)
        throw runtime_error("variant not found: " + variant_name);
}

void FiniteFieldRules::init(const YAML::Node& variant_node) {
    // --- 1 чтение основных параметров поля
    size_ = variant_node["size"].as<int>();
    zero_ = variant_node["zero_element"].as<string>()[0];
    one_  = variant_node["one_element"].as<string>()[0];

    // --- 2 чтение правила "плюс один"
    YAML::Node rpo = variant_node["rule_plus_one"];
    if (!rpo || !rpo.IsSequence())
        throw runtime_error("rule_plus_one must be a sequence");

    // --- 3 формирование последовательности символов поля
    vector<string> seq_strs = rpo.as<vector<string>>();
    vector<char> seq;
    seq.reserve(seq_strs.size());
    for (auto &s : seq_strs) {
        if (s.empty())
            throw runtime_error("empty string in rule_plus_one");
        seq.push_back(s[0]);
    }

    // --- 4 построение диаграммы Хассе
    map<char, char> next;
    for (int i = 0; i < size_ - 1; ++i) {
        next[seq[i]] = seq[i + 1];
    }
    next[seq.back()] = seq.front();     // замыкаем цикл

    // --- 5 обход цикла, формировани отображения символ -> индекс
    values_.clear();        // диаграмма Хассе (как массив а не мапа)
    char_to_val_.clear();   // отображение символ → индекс
    char cur = zero_;
    for (int i = 0; i < size_; ++i) {
        values_.push_back(cur);
        char_to_val_[cur] = i;
        cur = next[cur];
    }

    // --- 6 проверка корректности построения поля
    if ((int)values_.size() != size_) {
        throw runtime_error("incomplete mapping, got " +
            to_string(values_.size()) + ", expected " + to_string(size_));
    }
}

int FiniteFieldRules::getCharValue(char c) const {
    auto it = char_to_val_.find(c);
    if (it == char_to_val_.end())
        throw runtime_error("invalid character: " + string(1, c));
    return it->second;
}

