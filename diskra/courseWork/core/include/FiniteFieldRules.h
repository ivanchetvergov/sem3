#pragma once
#include <string>
#include <vector>
#include <map>
#include "yaml-cpp/yaml.h"

class FiniteFieldRules {
public:
    // * конструктор из файла конфигурации и имени варианта
    FiniteFieldRules(const std::string& config_file, const std::string& variant_name);

    // * свойства поля
    int  getSize() const { return size_; }
    char getZeroElement() const { return zero_; }
    char getOneElement() const { return one_; }

    // * преобразования между символами и значениями
    char getValueChar(int v) const { return values_.at(v % size_); }
    int  getCharValue(char c) const;

private:
    void init(const YAML::Node& variant_node);

    int size_;                              // размер поля
    char zero_;                             // нейтральный по сложению
    char one_;                              // нейтральный по умножению
    std::vector<char> values_;              // цикл элементов от zero_
    std::map<char,int> char_to_val_;        // символ → индекс 
};
