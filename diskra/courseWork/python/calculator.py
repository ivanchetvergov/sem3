# GUI/calculator.py
import os
import sys
import re
import tkinter as tk
from typing import Dict

sys.path.insert(0, 'build')
from finite_field_module import FiniteField, FiniteFieldRules  # type: ignore

class Calculator:
    def __init__(self, variant_name: str, widgets: Dict[str, tk.Widget]):
        """
        Cвязывает python gui с c++ backend.
        """
        self.widgets = widgets

        rules = FiniteFieldRules("config/core.yaml", variant_name)
        self.engine = FiniteField(rules)
        print(f"[ok] калькулятор инициализирован для варианта '{variant_name}'")

    def _evaluate_expression(self, op1: str, operator: str, op2: str) -> str:
        """
        Evaluates the expression with the C++ backend.
        """
        try:
            if operator == '+': result = self.engine.add(op1, op2)
            elif operator == '-': result = self.engine.subtract(op1, op2)
            elif operator == '*': result = self.engine.multiply(op1, op2)
            elif operator == '/': result = self.engine.divide(op1, op2)
            else: return "Ошибка: Неизвестный оператор"
            return str(result)
        except Exception as e:
            return f"Ошибка вычисления: {e}"


    def _to_ascii(self, char_str: str) -> str:
        return char_str.encode('ascii', 'replace').decode('ascii')

    def on_button_click(self, text: str):
        entry = self.widgets["entry_input"]
        label = self.widgets["label_output"]
        
        if text == "=":
            try:
                parts = re.split(r'(\+|-|\*|\/)', entry.get().strip())
                parts = [p for p in parts if p.strip()]

                if len(parts) != 3:
                    label.config(text="Ошибка: формат 'op1 op op2'")
                    return
                
                op1, operator, op2 = parts
                
                op1_ascii = self._to_ascii(op1)
                op2_ascii = self._to_ascii(op2)

                if op1 != op1_ascii or op2 != op2_ascii:
                    print(f"Внимание: символы {op1} и/или {op2} были изменены на {op1_ascii} и {op2_ascii}")

                try:
                    result = self._evaluate_expression(op1_ascii, operator, op2_ascii)
                    label.config(text=f"Результат: {result}")
                except Exception as e:
                    label.config(text=f"Ошибка: {e}")

            except Exception as e:
                label.config(text=f"Ошибка вычисления: {e}")
        elif text == "C":
            entry.delete(0, tk.END)
            label.config(text="Результат: ")
        else:
            entry.insert(tk.END, text)


