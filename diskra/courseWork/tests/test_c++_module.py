import pytest
import os
import sys

# Настройка пути и импорт модуля
sys.path.insert(0, os.path.abspath('build')) 

try:
    from finite_field_module import FiniteField, FiniteFieldRules # type: ignore
except ImportError as e:
    pytest.fail(f"Не удалось импортировать модуль: {e}. Убедитесь, что make отработал.")


@pytest.fixture(scope="session")
def field_calculator():
    """Инициализация объекта поля один раз за сессию."""
    try:
        rules = FiniteFieldRules("config/core.yaml", "variant_1")
        field = FiniteField(rules)
        return field
    except Exception as e:
        pytest.fail(f"Ошибка при инициализации: {e}")


def test_field_operations_comprehensive_internal(field_calculator):
    """
    Всеобъемлющий тест, который выполняет перебор всех пар 
    внутри функции (как в GTest), чтобы избежать проблем с параметризацией.
    """
    field = field_calculator
    rules = field.getRules()
    symbols = [rules.getValueChar(i) for i in range(rules.getSize())]
    zero = rules.getZeroElement()

    for a in symbols:
        for b in symbols:
            
            result_add_ab = field.add(a, b)
            result_add_ba = field.add(b, a)
            assert result_add_ab == result_add_ba, f"Add не коммутативно: {a} + {b} != {b} + {a}"

            field.subtract(a, b)

            result_mul_ab = field.multiply(a, b)
            result_mul_ba = field.multiply(b, a)
            assert result_mul_ab == result_mul_ba, f"Multiply не коммутативно: {a} * {b} != {b} * {a}"

            if b == zero:
                with pytest.raises(Exception):
                    field.divide(a, b)
                continue
            
            try:
                field.findMultiplicativeInverse(b)
                
                result_div = field.divide(a, b)
                assert field.multiply(result_div, b) == a, f"Деление провалено: ({a} / {b}) * {b} != {a}"
                
            except Exception:
                with pytest.raises(Exception):
                    field.divide(a, b)
                    
    assert True