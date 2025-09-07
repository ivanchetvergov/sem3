#include "UIHandler.h"
#include "IO.h"
#include "Core.h"

using std::cout;

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
        cout << "ошибка: разрядность не может быть отрицательной.\n";
        return;
    }
    universe_.fillRandomly(n);
    cout << "универсум успешно сгенерирован.\n";
    universe_.print();
}

void UIHandler::handleManualFill() {
    if (universe_.isEmpty()) {
        cout << "ошибка: сначала сгенерируйте универсум (опция 1).\n";
        return;
    }
    cout << "--- заполнение множества A ---\n";
    A_.fillManually(universe_);
    cout << "--- заполнение множества B ---\n";
    B_.fillManually(universe_);
}

void UIHandler::handleAutomaticFill() {
    if (universe_.isEmpty()) {
        cout << "ошибка: сначала сгенерируйте универсум (опция 1).\n";
        return;
    }
    cout << "--- автоматическое заполнение множества A ---\n";
    int cardinalityA = readInteger("введите желаемую мощность для A: ");
    A_.fillAutomatically(universe_, cardinalityA);
    cout << "--- автоматическое заполнение множества B ---\n";
    int cardinalityB = readInteger("введите желаемую мощность для B: ");
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
        std::cout << "ошибка: сначала заполните множества A и B (опция 2 или 3).\n";
        return;
    }
    
    while (true) {
        showOperationsMenu();
        int choice = readInteger("выберите операцию: ");
        Multiset result; // Создаём мультимножество для хранения результата

        switch (choice) {
            case 1:
                result = A_ | B_;
                std::cout << "\n--- результат объединения ---\n";
                result.print();
                break;
            case 2:
                result = A_ & B_;
                std::cout << "\n--- результат пересечения ---\n";
                result.print();
                break;
            case 3:
                result = universe_ - A_;
                std::cout << "\n--- результат дополнения ---\n";
                result.print();
                break;
            case 4:
                result = A_ - B_;
                std::cout << "\n--- результат разности ---\n";
                result.print();
                break;
            case 5:
                result = A_ ^ B_;
                std::cout << "\n--- результат симметрической разности ---\n";
                result.print();
                break;
            case 6:
                return;
            default:
                std::cout << "неверный выбор. попробуйте снова.\n";
        }
    }
}

void UIHandler::handleMainMenu() {
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
            cout << "неверный выбор. попробуйте снова.\n";
    }
}

void UIHandler::run() {
    while (true) {
        this->handleMainMenu();
    }
}