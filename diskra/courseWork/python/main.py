# GUI/main.py
import sys
from python.gui_builder import load_config, build_gui
from python.calculator import Calculator
from typing import Dict
import tkinter as tk

def main():
    config = load_config("config/gui.yaml")
    if not config:
        print("Не удалось загрузить конфигурацию. Завершение работы.")
        sys.exit(1)

    widgets: Dict[str, tk.Widget] = {}
    
    try:
        calc = Calculator("variant_1", widgets)
    except Exception as e:
        print(f"Критическая ошибка: {e}")
        sys.exit(1)
    
    build_gui(config, handler=calc.on_button_click, widgets=widgets)
    
    if "window" not in widgets:
        print("Ошибка: GUI не был создан. Отсутствует виджет 'window'.")
        sys.exit(1)

    widgets["window"].mainloop()

if __name__ == "__main__":
    main()