# python/calculator.py
import sys
import os
import yaml
import re
from typing import Dict, Any, List, Optional

# Добавляем путь к C++ модулю
sys.path.insert(0, 'build')
try:
    from finite_ring_module import FiniteRingRules, SmallRingArithmetic, BigRingArithmetic, RingNumber # type: ignore
except ImportError:
    print("--- ОШИБКА: Не удалось импортировать 'с++ модуль'.")
    sys.exit(1)


class Calculator:
    """
    Интерактивный калькулятор для конечных колец (Z_n),
    использующий BigRingArithmetic.
    """
    MAX_DIGITS = 8 
    
    def __init__(self, variant_name: str):
        self._print_header(variant_name)
        
        try:
            # 1. Инициализация правил
            self.rules = FiniteRingRules("config.yaml", variant_name)
            
            # 2. Инициализация движка малой арифметики (зависимость)
            self.small_engine = SmallRingArithmetic(self.rules)
            
            # 3. Инициализация главного движка большой арифметики
            self.engine = BigRingArithmetic(self.rules, self.small_engine)
            
            # Получаем свойства кольца
            self.size: int = self.rules.getSize()
            self.zero: str = self.rules.getZeroElement()
            self.one: str = self.rules.getOneElement()
            
            # Получаем все символы для валидации
            self.symbols: List[str] = [self.rules.getValueChar(i) for i in range(self.size)]
            
            self._print_ring_info()
            
        except Exception as e:
            print(f"--- ОШИБКА инициализации: {e}")
            print(f"Проверьте наличие файла 'config.yaml' и имя варианта '{variant_name}'.")
            sys.exit(1)
            
    def _print_header(self, variant_name: str):
        """Выводит заголовок."""
        separator = f"{'='*50}"
        print(f"\n{separator}")
        print(f"Инициализация калькулятора для варианта: {variant_name}")
        print(f"{separator}\n")

    def _print_ring_info(self):
        """Выводит информацию о параметрах кольца."""
        separator = f"{'='*50}"
        print(f" --- Размер кольца: Z{self.size}")
        print(f" --- Нулевой элемент: {self.zero}")
        print(f" --- Единичный элемент: {self.one}")
        print(f"# Символы кольца: {self.symbols}")
        print(f"\n{separator}\n")
    
    def _parse_expression(self, expression: str) -> Optional[List[str]]:
        """
        Парсит выражение, поддерживая унарный минус: A * -B, -A + B.
        Возвращает [op1_str, operator, op2_str] или None в случае ошибки.
        """
        # Регулярное выражение для поиска: [возможно минус][число] [оператор] [возможно минус][число]
        # Используем \s* для обработки пробелов
        pattern = r"^(-?\w+)\s*([+\-*/])\s*(-?\w+)$"
        match = re.match(pattern, expression.strip())
        
        if match:
            # Возвращает op1 (включая минус), operator, op2 (включая минус)
            return list(match.groups())
        
        return None

    def calculate(self, op1_full: str, operator: str, op2_full: str) -> str:
        """
        Обрабатывает унарный минус и выполняет вычисление.
        Возвращает строковый результат или сообщение об ошибке.
        """
        
        # 1. Обработка унарного минуса (п. 8)
        
        # Парсинг op1
        is_neg_1 = op1_full.startswith('-')
        op1_str = op1_full.lstrip('-')
        
        # Парсинг op2
        is_neg_2 = op2_full.startswith('-')
        op2_str = op2_full.lstrip('-')

        # 2. Конвертируем в RingNumber
        try:
            # C++ RingNumber теперь принимает только символы кольца
            num1 = RingNumber(self.rules, op1_str)
            num2 = RingNumber(self.rules, op2_str)
            
            # 3. Применяем унарный минус через C++ движок
            if is_neg_1:
                num1 = self.engine.negate(num1)
            if is_neg_2:
                num2 = self.engine.negate(num2)

        except RuntimeError as e:
            # RingNumber бросает RuntimeError, если видит недопустимый символ (п. 6)
            return f"Ошибка ввода: {e}"
        except Exception as e:
            return f"Ошибка ввода (Непредвиденная): {e}"

        # 4. Выполнение операции
        try:
            if operator == '+':
                result_obj = self.engine.add(num1, num2)
            elif operator == '-':
                result_obj = self.engine.subtract(num1, num2)
            elif operator == '*':
                result_obj = self.engine.multiply(num1, num2)
            elif operator == '/':
                result_obj = self.engine.divide(num1, num2)
            else:
                # На этот случай _parse_expression не должен допускать
                return f"Ошибка: неизвестный оператор '{operator}'."
            
            # 5. Конвертируем RingNumber обратно в строку для вывода
            return result_obj.toString()
            
        except RuntimeError as e:
            # Перехватываем исключения C++ (деление на ноль, нет обратного)
            return f"Ошибка вычисления (RuntimeError): {e}"
        except Exception as e:
            return f"Ошибка вычисления: {e}"
    
    def _display_help(self):
        """Выводит справку."""
        print("\n--- СПРАВКА ---")
        print("Формат ввода: <число> <оператор> <число> (Например: cab * -bac)")
        print("Поддерживаемые операторы: +, -, *, /")
        print("Числа могут начинаться с минуса (унарный минус: -bac)")
        print("\nСпециальные команды:")
        print("  /rules - Показать правила кольца (отношение порядка, таблица индексов)")
        print("  /help  - Показать эту справку")
        print("  exit   - Выход из программы\n")
        
    def run(self):
        """Запускает интерактивный режим."""
        print("\n==========================================")
        print("Калькулятор запущен!")
        print(f"Кольцо Z{self.size}. Символы: {' '.join(self.symbols)}")
        print(f"Операции: +  -  * /")
        print("Введите /help для справки.")
        print("==========================================")
        
        while True:
            try:
                expression = input(">>> ").strip()
                
                if not expression:
                    continue
                
                # --- Специальные команды ---
                if expression.lower() in ['exit', 'quit', 'q']:
                    print("\nДо свидания!\n")
                    break
                
                if expression.lower() == '/rules':
                    # Вызов метода C++ для вывода правил
                    self.rules.printRules() 
                    continue
                    
                if expression.lower() == '/help':
                    self._display_help()
                    continue

                # --- Обработка ввода одного символа (индекса) ---
                parts = expression.split()
                if len(parts) == 1 and parts[0] not in ['/rules', '/help']:
                    sym = parts[0]
                    try:
                        idx = self.rules.getCharValue(sym)
                        print(f"  -> '{sym}' имеет индекс {idx}")
                    except RuntimeError:
                        print(f"  → Ошибка: Символ '{sym}' не является элементом кольца.")
                    continue
                
                # --- Парсинг и вычисление выражения ---
                parsed = self._parse_expression(expression)
                
                if not parsed:
                    print(f"  → Ошибка формата! Используйте: число оператор число (например, cab * -bac)")
                    continue
                
                op1_full, operator, op2_full = parsed
                
                # Вычисление
                result = self.calculate(op1_full, operator, op2_full)
                
                # Вывод результата
                if not result.startswith("Ошибка"):
                    
                    # ПРЕДУПРЕЖДЕНИЕ О ПЕРЕПОЛНЕНИИ (п. 5)
                    if len(result) > self.MAX_DIGITS:
                        print(f"  → !!! ВНИМАНИЕ: Произошло переполнение (разрядность > {self.MAX_DIGITS}). Результат может быть некорректным.")
                    
                    print(f"  → {op1_full} {operator} {op2_full} = {result}")
                else:
                    print(f"  → {result}")
                    
            except KeyboardInterrupt:
                print("\n\nПрервано пользователем. До свидания!\n")
                break
            except Exception as e:
                print(f"  → Неожиданная ошибка Python: {e}")
                    
def load_variants() -> List[Dict[str, Any]]:
    """
    Загружает список доступных вариантов из config.yaml.
    Возвращает список словарей с описанием вариантов.
    """
    try:
        if not os.path.exists("config.yaml"):
             raise FileNotFoundError("Файл 'config.yaml' не найден.")
             
        with open("config.yaml", "r", encoding="utf-8") as f:
            config = yaml.safe_load(f) or {}
            variants: List[Dict[str, Any]] = []
            
            # Безопасный обход структуры YAML
            for field_type, field_variants in config.get("variants", {}).items():
                if isinstance(field_variants, dict):
                    for variant_name, variant_data in field_variants.items():
                        if isinstance(variant_data, dict):
                            size = variant_data.get("size", "?")
                            symbols = variant_data.get("rule_plus_one", [])
                            variants.append({
                                'name': variant_name,
                                'type': field_type,
                                'size': size,
                                'symbols': symbols
                            })
            return variants
            
    except Exception as e:
        print(f"--- ОШИБКА загрузки вариантов из config.yaml: {e}")
        return []

if __name__ == '__main__':
    print("Error, приложение запускается через python -m python.main из корневой директории проекта")