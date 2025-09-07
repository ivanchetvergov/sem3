// UIHandler.cc
#include "UIHandler.h"
#include "Multiset.h"
#include "IO.h"
#include "Exceptions.h"
#include <iostream>
#include <stdexcept>

using std::cout;
using std::cerr;

UIHandler::UIHandler(Multiset& universe, Multiset& A, Multiset& B)
    : universe_(universe), A_(A), B_(B) {}

void UIHandler::showMenu() {
    cout << "\n--- главное меню ---\n";
    cout << "1. сгенерировать универсум мультимножеств\n";
    cout << "2. заполнить множества A и B вручную\n";
    cout << "3. заполнить множества A и B автоматически\n";
    cout << "4. выполнить операции над множествами\n";
    cout << "5. выход\n";
    cout << "---------------------\n";
}

void UIHandler::handleGenerateUniverse() {
    int n = readInteger("введите разрядность (n): ");
    if (n < 0) {
        throw InvalidValueException("отрицательная разрядность недопустима.");
    }
    universe_.fillRandomly(n);
    cout << "универсум успешно сгенерирован.\n";
    universe_.print();
}

void UIHandler::handleManualFill() {
    if (universe_.isEmpty()) {
        throw InvalidOperationException("универсум пуст. сначала сгенерируйте его (опция 1).");
    }
    cout << "--- заполнение множества A ---\n";
    A_.fillManually(universe_);
    cout << "--- заполнение множества B ---\n";
    B_.fillManually(universe_);
}

void UIHandler::handleAutomaticFill() {
    if (universe_.isEmpty()) {
        throw InvalidOperationException("универсум пуст. сначала сгенерируйте его (опция 1).");
    }
    cout << "--- автоматическое заполнение множества A ---\n";
    int cardinalityA = readInteger("введите желаемую мощность для A: ");
    if (cardinalityA > universe_.getCardinality()) {
        throw InvalidValueException("желаемая мощность для A превышает мощность универсума.");
    }
    A_.fillAutomatically(universe_, cardinalityA);
    cout << "--- автоматическое заполнение множества B ---\n";
    int cardinalityB = readInteger("введите желаемую мощность для B: ");
    if (cardinalityB > universe_.getCardinality()) {
        throw InvalidValueException("желаемая мощность для B превышает мощность универсума.");
    }
    B_.fillAutomatically(universe_, cardinalityB);
}

void UIHandler::showOperationsMenu() {
    cout << "\n--- подменю операций ---\n";
    cout << "1. объединение (A | B)\n";
    cout << "2. пересечение (A & B)\n";
    cout << "3. дополнение к A (U - A)\n";
    cout << "4. разность (A - B)\n";
    cout << "5. симметрическая разность (A ^ B)\n";
    cout << "6. возвращение в главное меню\n";
    cout << "-------------------------\n";
}

void UIHandler::handleOperationsMenu() {
    if (A_.isEmpty() || B_.isEmpty()) {
        throw InvalidOperationException("множества A и B еще не заполнены. сначала заполните их (опция 2 или 3).");
    }
    
    while (true) {
        try {
            showOperationsMenu();
            int choice = readInteger("выберите операцию: ");
            Multiset result;

            switch (choice) {
                case 1:
                    result = A_ | B_;
                    cout << "\n--- результат объединения ---\n";
                    result.print();
                    break;
                case 2:
                    result = A_ & B_;
                    cout << "\n--- результат пересечения ---\n";
                    result.print();
                    break;
                case 3:
                    result = universe_ - A_;
                    cout << "\n--- результат дополнения ---\n";
                    result.print();
                    break;
                case 4:
                    result = A_ - B_;
                    cout << "\n--- результат разности ---\n";
                    result.print();
                    break;
                case 5:
                    result = A_ ^ B_;
                    cout << "\n--- результат симметрической разности ---\n";
                    result.print();
                    break;
                case 6:
                    return;
                default:
                    throw InvalidValueException("неверный выбор.");
            }
        }
        catch (const InvalidOperationException& e) {
            cerr << e.what() << '\n';
        }
        catch (const MultisetException& e) {
            cerr << e.what() << '\n';
        }
        catch (const std::exception& e) {
            cerr << "произошла непредвиденная ошибка: " << e.what() << '\n';
        }
    }
}

void UIHandler::handleMainMenu() {
    try {
        this->showMenu();
        int choice = readInteger("выберите операцию: ");

        switch (choice) {
            case 1:
                this->handleGenerateUniverse();
                break;
            case 2:
                this->handleManualFill();
                break;
            case 3:
                this->handleAutomaticFill();
                break;
            case 4:
                this->handleOperationsMenu();
                break;
            case 5:
                cout << "выход из программы.\n";
                exit(0);
            default:
                throw InvalidValueException("неверный выбор.");
        }
    }
    catch (const MultisetException& e) {
        cerr << e.what() << '\n';
    }
    catch (const std::exception& e) {
        cerr << "произошла непредвиденная ошибка: " << e.what() << '\n';
    }
}

void UIHandler::run() {
    while (true) {
        this->handleMainMenu();
    }
}