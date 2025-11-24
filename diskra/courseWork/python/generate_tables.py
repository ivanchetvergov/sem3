import sys
import os
from typing import List

# Проверяем, что скрипт запускается из нужной директории
if not os.path.exists('config.yaml'):
    print("Ошибка: Файл 'config.yaml' не найден. Запускайте скрипт из корневой директории проекта.")
    sys.exit(1)

sys.path.insert(0, 'build')

try:
    from finite_ring_module import (
        FiniteRingRules, 
        SmallRingArithmetic, 
        BigRingArithmetic, 
        RingNumber, 
        DivisionResult
    )
except ImportError as e:
    print(f"Ошибка импорта модуля: {e}")
    print("Убедитесь, что C++ модуль скомпилирован и находится в директории 'build'.")
    sys.exit(1)


# --- Основная логика генерации таблиц ---

VARIANT_NAME = "D9" # Выбранный вариант D9
rules = FiniteRingRules("config.yaml", VARIANT_NAME)
small = SmallRingArithmetic(rules)

symbols = [rules.getValueChar(i) for i in range(rules.getSize())]
SIZE = rules.getSize()

# Увеличенная ширина ячейки для идеального зазора (например, 'a  ')
CELL_WIDTH = 3

def print_table(operation_name: str, symbols: List[str], operation_func):
    """Генерирует и выводит таблицу для заданной операции, обеспечивая идеальное выравнивание."""
    
    # 1. Заголовочная строка с символами
    
    # Первая колонка заголовка должна быть пустой и иметь ширину CELL_WIDTH.
    header_start = " " * CELL_WIDTH + "|"
    
    # Элементы заголовка, выровненные по CELL_WIDTH.
    # Обратите внимание: объединяем их БЕЗ ДОПОЛНИТЕЛЬНОГО ПРОБЕЛА.
    header_symbols = "".join([sym.ljust(CELL_WIDTH) for sym in symbols])
    
    print(f"\n {" " * 8} Таблица {operation_name}:\n")
    print(f"{header_start} {header_symbols}")
    
    # 2. Разделительная линия
    # Длина линии данных = CELL_WIDTH * SIZE
    data_line_length = CELL_WIDTH * SIZE
    
    separator = "-" * CELL_WIDTH + "+" + "-" * (data_line_length + 1) # +1 для пробела после '|'
    print(separator)

    # 3. Строки данных
    for a in symbols:
        # Первая ячейка (заголовок строки)
        row_start = a.ljust(CELL_WIDTH) + "|"
        
        # Элементы данных
        data_cells = []
        for b in symbols:
            result = operation_func(a, b)
            result_char = result[-1] 
            data_cells.append(result_char.ljust(CELL_WIDTH))
        
        # Объединяем элементы данных, как и в заголовке, БЕЗ ДОПОЛНИТЕЛЬНОГО ПРОБЕЛА
        print(f"{row_start} {''.join(data_cells)}")

# --- Вывод ---

print("==========================================")
print(f"Генерация таблиц для кольца Z{SIZE} (D9)")
print("==========================================")

# Таблица сложения
print_table("сложения", symbols, small.add)

# Таблица умножения
print_table("умножения", symbols, small.multiply)

print("\n==========================================")