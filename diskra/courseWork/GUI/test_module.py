import sys
import os


sys.path.insert(0, os.path.abspath('../build'))

try:
    from finite_field_module import FiniteField, FiniteFieldRules # type: ignore 

    rules = FiniteFieldRules("../core/core_config.yaml", "variant_1")
    calculator = FiniteField(rules)

    zero = rules.getZeroElement()
    one = rules.getOneElement()

    result_add = calculator.add(one, zero)
    assert result_add == one, f"Addition failed! Expected {one}, got {result_add}"
    print(f"Addition test passed: {one} + {zero} = {result_add}")

    result_sub = calculator.subtract(one, one)
    assert result_sub == zero, f"Subtraction failed! Expected {zero}, got {result_sub}"
    print(f"Subtraction test passed: {one} - {one} = {result_sub}")

    result_mul = calculator.multiply(one, zero)
    assert result_mul == zero, f"Multiplication failed! Expected {zero}, got {result_mul}"
    print(f"Multiplication test passed: {one} * {zero} = {result_mul}")

    result_div = calculator.divide(one, one)
    assert result_div == one, f"Division failed! Expected {one}, got {result_div}"
    print(f"Division test passed: {one} / {one} = {result_div}")

    print("\nAll test's passed!")

except Exception as e:
    print(f"error: {e}")