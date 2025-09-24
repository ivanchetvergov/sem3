# main.py
import yaml
import tkinter as tk
from GUI.gui_builder import build_gui

def load_config(file_path: str) -> dict | None:
    """
    Загружает конфигурацию из YAML-файла.

    :param file_path: Путь к файлу конфигурации.
    :return: Словарь с данными или None в случае ошибки.
    """
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            return yaml.safe_load(file)
    except FileNotFoundError:
        print(f"Ошибка: Файл конфигурации '{file_path}' не найден.")
        return None
    except yaml.YAMLError as e:
        print(f"Ошибка парсинга YAML: {e}")
        return None

def on_button_click(button_text: str):
    """
    Обработчик нажатия на кнопки. Эта функция будет обрабатывать
    ввод пользователя и вызывать функции вычисления.

    :param button_text: Текст кнопки, на которую нажали.
    """
    current_expression = widgets['entry_input'].get()
    
    if button_text == '=':
        # Здесь будет логика для вызова C++ бэкенда.
        # Например: result = calculator_core.evaluate(current_expression)
        print(f"Вычисляем: {current_expression}")
        widgets['label_output'].config(text="Вычисляем...")
    elif button_text == 'C':
        widgets['entry_input'].delete(0, tk.END)
        widgets['label_output'].config(text="Результат: ")
    else:
        widgets['entry_input'].insert(tk.END, button_text)

if __name__ == "__main__":
    config = load_config('GUI/config.yaml')
    
    if config:

        widgets = build_gui(config, on_button_click)

        widgets['window'].mainloop()