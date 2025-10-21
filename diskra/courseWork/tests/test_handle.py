import sys
import os


sys.path.insert(0, os.path.abspath('build'))

try:
    from finite_field_module import FiniteField, FiniteFieldRules  # type: ignore 

    rules = FiniteFieldRules("config.yaml", "variant_1")
    calculator = FiniteField(rules)

    zero = rules.getZeroElement()
    one = rules.getOneElement()

    # выводим типы данных
    print(f"Type of zero: {type(zero)}, : {zero}, len = {len(zero)}")
    print(f"Type of one: {type(one)} : {one}, len = {len(one)}")

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
    
    alphabet = [rules.getValueChar(i) for i in range(rules.getSize())]
    print("alphabet =", alphabet)
    
    for a in alphabet:
        for b in alphabet:
            try:
                print(f"{a}+{b} = {calculator.add(a, b)}")
            except Exception as e:
                print(f"add({a},{b}) failed: {e}")
            try:
                print(f"{a}-{b} = {calculator.subtract(a, b)}")
            except Exception as e:
                print(f"sub({a},{b}) failed: {e}")
            try:
                print(f"{a}*{b} = {calculator.multiply(a, b)}")
            except Exception as e:
                print(f"mul({a},{b}) failed: {e}")
            try:
                print(f"{a}/{b} = {calculator.divide(a, b)}")
            except Exception as e:
                print(f"div({a},{b}) failed: {e}")
        print("-" * 20)


except Exception as e:
    print(f"error: {e}")
