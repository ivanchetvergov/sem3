# python/calculator.py
import sys
import os
import yaml

sys.path.insert(0, 'build')
from finite_field_module import FiniteField, FiniteFieldRules  # type: ignore


class Calculator:
    def __init__(self, variant_name: str):
        print(f"\n{'='*50}")
        print(f"Инициализация калькулятора для варианта: {variant_name}")
        print(f"{'='*50}\n")
        
        try:
            self.rules = FiniteFieldRules("config/core.yaml", variant_name)
            self.engine = FiniteField(self.rules)
            
            # Получаем информацию о поле
            self.size = self.rules.getSize()
            self.zero = self.rules.getZeroElement()
            self.one = self.rules.getOneElement()
            
            print(f" --- Размер поля: Z{self.size}")
            print(f" --- Нулевой элемент: {self.zero}")
            print(f" --- Единичный элемент: {self.one}")
            
            # Получаем все символы
            self.symbols = []
            for i in range(self.size):
                sym = self.rules.getValueChar(i)
                self.symbols.append(sym)
            
            print(f"✓ Символы поля: {self.symbols}")
            print(f"\n{'='*50}\n")
            
        except Exception as e:
            print(f"✗ ОШИБКА инициализации: {e}")
            sys.exit(1)
    
    def calculate(self, op1: str, operator: str, op2: str) -> str:
        """Выполняет вычисление."""
        try:
            # Проверяем что символы валидны
            if op1 not in self.symbols:
                return f"Ошибка: символ '{op1}' не существует в поле. Доступные: {self.symbols}"
            if op2 not in self.symbols:
                return f"Ошибка: символ '{op2}' не существует в поле. Доступные: {self.symbols}"
            
            if operator == '+':
                result = self.engine.add(op1, op2)
            elif operator == '-':
                result = self.engine.subtract(op1, op2)
            elif operator == '*':
                result = self.engine.multiply(op1, op2)
            elif operator == '/':
                result = self.engine.divide(op1, op2)
            else:
                return f"Ошибка: неизвестный оператор '{operator}'. Доступные: +, -, *, /"
            
            return result
            
        except Exception as e:
            return f"Ошибка вычисления: {e}"
    
    def run(self):
        """Запускает интерактивный режим."""
        print("Калькулятор запущен!")
        print(f"Доступные символы: {' '.join(self.symbols)}")
        print(f"Доступные операции: +  -  *  /")
        print(f"\nВводите выражения в формате: a + b")
        print(f"Для выхода введите: exit или quit\n")
        
        while True:
            try:
                expression = input(">>> ").strip()
                
                if expression.lower() in ['exit', 'quit', 'q']:
                    print("\nДо свидания! 👋\n")
                    break
                
                if not expression:
                    continue
                
                # Парсим выражение
                parts = expression.split()
                
                if len(parts) == 1:
                    # Просто символ - показываем его индекс
                    if parts[0] in self.symbols:
                        idx = self.rules.getCharValue(parts[0])
                        print(f"  → '{parts[0]}' имеет индекс {idx}")
                    else:
                        print(f"  → Символ '{parts[0]}' не найден в поле")
                    continue
                
                if len(parts) != 3:
                    print(f"  → Ошибка формата! Используйте: символ оператор символ")
                    print(f"     Например: a + b")
                    continue
                
                op1, operator, op2 = parts
                
                # Вычисляем
                result = self.calculate(op1, operator, op2)
                
                # Красивый вывод
                if not result.startswith("Ошибка"):
                    print(f"  → {op1} {operator} {op2} = {result}")
                else:
                    print(f"  → {result}")
                    
            except KeyboardInterrupt:
                print("\n\nПрервано пользователем. До свидания! 👋\n")
                break
            except Exception as e:
                print(f"  → Неожиданная ошибка: {e}")
                    
def load_variants():
    """Загружает список доступных вариантов."""
    try:
        with open("config/core.yaml", "r", encoding="utf-8") as f:
            config = yaml.safe_load(f)
            variants = []
            for field_type, field_variants in config.get("variants", {}).items():
                for variant_name, variant_data in field_variants.items():
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
        print(f"Ошибка загрузки вариантов: {e}")
        return []

