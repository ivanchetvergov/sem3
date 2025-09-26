# GUI/gui_builder.py
import tkinter as tk
from tkinter import ttk
import yaml
from typing import Dict, Callable

def load_config(path: str) -> dict | None:
    try:
        with open(path, "r", encoding="utf-8") as f:
            return yaml.safe_load(f)
    except Exception as e:
        print(f"[err] загрузка {path}: {e}")
        return None

def build_gui(config: dict, handler: Callable[[str], None], widgets: Dict[str, tk.Widget]):
    """
    Создает GUI и заполняет переданный словарь виджетов.
    """
    window = tk.Tk()
    window.title(config["window"]["title"])
    window.geometry(config["window"]["geometry"])
    window.resizable(config["window"]["resizable_width"],
                     config["window"]["resizable_height"])
    window.config(bg=config["colors"]["background"])
    widgets['window'] = window
    
    style = ttk.Style()
    style.theme_use("clam")
    
    style.configure("TFrame", background=config["colors"]["background"])
    style.configure("TEntry",
                    fieldbackground=config["colors"]["display_background"],
                    foreground=config["colors"]["text"],
                    font=(config["fonts"]["name"], config["fonts"]["size_display"]))
    style.configure("TLabel",
                    background=config["colors"]["background"],
                    foreground=config["colors"]["text"],
                    font=(config["fonts"]["name"], config["fonts"]["size_button"]),
                    anchor="e")

    entry_input = ttk.Entry(window, style="TEntry", justify="right")
    entry_input.pack(fill="x", padx=10, pady=10, ipady=10)
    widgets["entry_input"] = entry_input

    label_output = ttk.Label(window, text="результат: ", style="TLabel")
    label_output.pack(fill="x", padx=10, pady=(0, 10))
    widgets["label_output"] = label_output

    button_frame = ttk.Frame(window, style="TFrame")
    button_frame.pack(expand=True, fill="both", padx=10, pady=5)

    for btn in config["buttons"]:
        text = btn["text"]
        if not text:
            continue

        row, col, _, _ = btn["grid"]
        colspan = btn.get("colspan", 1)

        b = ttk.Button(button_frame, text=text, style=btn["style"],
                       command=lambda t=text: handler(t))
        b.grid(row=row, column=col, columnspan=colspan,
               sticky="nsew", padx=2, pady=2)

    for i in range(4):
        button_frame.grid_columnconfigure(i, weight=1)
    for i in range(len(config["buttons"]) // 4 + 1):
        button_frame.grid_rowconfigure(i, weight=1)