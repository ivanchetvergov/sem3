# GUI/gui_builder.py
import tkinter as tk
from tkinter import ttk

def build_gui(config: dict, on_button_click_handler) -> dict:
    """
    Создает и настраивает GUI на основе конфигурации, загруженной из YAML.

    :param config: Словарь с настройками, загруженный из config.yaml.
    :param on_button_click_handler: Функция-обработчик, которая будет вызываться
                                     при нажатии на кнопки.
    :return: Словарь, содержащий ссылки на созданные виджеты (окно, поля ввода/вывода).
    """
    # 1. Создание и базовая настройка главного окна
    # Используем данные из секции 'window' конфига.
    window = tk.Tk()
    window.title(config['window']['title'])
    window.geometry(config['window']['geometry'])
    window.resizable(config['window']['resizable_width'], config['window']['resizable_height'])
    window.config(bg=config['colors']['background'])

    # 2. Настройка стилей для ttk-виджетов
    # Эти стили управляют внешним видом всех кнопок, полей и т.д.
    style = ttk.Style()
    style.theme_use('clam')
    
    style.configure('TFrame', background=config['colors']['background'])
    style.configure('TEntry', fieldbackground=config['colors']['display_background'],
                    foreground=config['colors']['text'],
                    font=(config['fonts']['name'], config['fonts']['size_display']),
                    borderwidth=0, relief='flat')
    style.configure('TLabel', background=config['colors']['background'],
                    foreground=config['colors']['text'],
                    font=(config['fonts']['name'], config['fonts']['size_button']),
                    anchor='e')
    style.configure('TButton', background=config['colors']['button'],
                    foreground=config['colors']['text'],
                    font=(config['fonts']['name'], config['fonts']['size_button']),
                    borderwidth=0, relief='flat', padding=(10, 10))
    style.map('TButton', background=[('active', config['colors']['active_button'])])
    style.configure('Operator.TButton', background=config['colors']['operator'], foreground=config['colors']['text'])
    style.map('Operator.TButton', background=[('active', config['colors']['active_operator'])])
    style.configure('Clear.TButton', background=config['colors']['clear'], foreground=config['colors']['background'])
    style.map('Clear.TButton', background=[('active', config['colors']['active_clear'])])

    # 3. Создание основных виджетов (дисплей)
    entry_input = ttk.Entry(window, style='TEntry', justify='right')
    entry_input.pack(fill='x', padx=10, pady=10, ipady=10)

    label_output = ttk.Label(window, text="Результат: ", style='TLabel')
    label_output.pack(fill='x', padx=10, pady=(0, 10))

    # 4. Создание фрейма для кнопок и их динамическое размещение
    button_frame = ttk.Frame(window, style='TFrame')
    button_frame.pack(expand=True, fill='both', padx=10, pady=5)

    for btn_data in config['buttons']:
        btn_text = btn_data['text']
        btn_style = btn_data['style']
        grid_params = btn_data['grid']

        if btn_text == '':
            continue

        # Привязываем lambda-функцию, которая вызывает переданный обработчик.
        button = ttk.Button(button_frame, text=btn_text, style=btn_style,
                            command=lambda t=btn_text: on_button_click_handler(t))

        row, col, _, _ = grid_params
        colspan = btn_data.get('colspan', 1)
        button.grid(row=row, column=col, columnspan=colspan, sticky='nsew', padx=2, pady=2)

    # 5. Настройка растягивания колонок и рядов для адаптивности
    for i in range(4):
        button_frame.grid_columnconfigure(i, weight=1)
    for i in range(len(config['buttons']) // 4 + 1):
        button_frame.grid_rowconfigure(i, weight=1)

    # 6. Возвращаем словарь с виджетами для доступа извне
    return {
        'window': window,
        'entry_input': entry_input,
        'label_output': label_output
    }
