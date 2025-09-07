// Multiset_Fill.cc

#include "Multiset.h"
#include "Core.h"
#include "IO.h"
#include <random>

void Multiset::fillRandomly(int n) {
    elements_.clear();
    totalCardinality_ = 0;

    std::vector<std::string> grayCodes = generateGrayCode(n);
    if (grayCodes.empty() && n != 0) { return; }
    
    // используем более современный вариант rand() 
    std::random_device rd;                              // источник энтропии
    std::mt19937 gen(rd());                             // генератор (seed(энтропии))
    std::uniform_int_distribution<> distrib(1, 100);    // равномерное распределние велечин

    for (const auto& code : grayCodes) {
        int cardinality = distrib(gen);
        elements_[code] = cardinality;
        totalCardinality_ += cardinality;
    }
}

void Multiset::fillManually(const Multiset& universe) {
    elements_.clear();
    totalCardinality_ = 0;

    for (const auto& pair : universe.getElements()) {
        const std::string& code = pair.first;
        const int max_cardinality = pair.second;
        
        int current_cardinality;
        
        while (true) {
            current_cardinality = readInteger("  Введите кратность для кода " + code + " (max: " + std::to_string(max_cardinality) + "): ");
            
            if (current_cardinality >= 0 && current_cardinality <= max_cardinality) {
                if (current_cardinality > 0) {
                    elements_[code] = current_cardinality;
                    totalCardinality_ += current_cardinality;
                }
                break;
            } else {
                std::cout << "Ошибка: Кратность должна быть >= 0 и <= " << max_cardinality << ".\n";
            }
        }
    }
}

void Multiset::fillAutomatically(const Multiset& universe, int desiredCardinality) {
    elements_.clear();
    totalCardinality_ = 0;

    long long universeCardinality = universe.getCardinality();
    if (desiredCardinality < 0 || desiredCardinality > universeCardinality) {
        std::cout << "Ошибка: Желаемая мощность должна быть от 0 до " << universeCardinality << ".\n";
        return;
    }
    if (desiredCardinality == 0) {
        return;
    }

    // Заполняем элементы кратностями, не превышающими универсум
    std::map<std::string, int> temp_elements;
    for (const auto& pair : universe.getElements()) {
        temp_elements[pair.first] = 0;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, universe.getElements().size() - 1);
    
    std::vector<std::string> codes;
    for(const auto& pair : universe.getElements()) {
        codes.push_back(pair.first);
    }
    
    // Добавляем элементы по одному, пока не достигнем нужной мощности
    for (int i = 0; i < desiredCardinality; ++i) {
        int randomIndex = distrib(gen);
        const std::string& code = codes[randomIndex];

        if (temp_elements[code] < universe.getElements().at(code)) {
            temp_elements[code]++;
        } else {
            // Если кратность уже максимальная, пробуем снова
            i--;
        }
    }

    // Переносим результаты в основной контейнер и обновляем мощность
    for (const auto& pair : temp_elements) {
        if (pair.second > 0) {
            elements_[pair.first] = pair.second;
        }
    }
    totalCardinality_ = desiredCardinality;
}