from python.calculator import Calculator, load_variants
import sys

def show_menu():
    """Показывает меню выбора варианта."""
    variants = load_variants()
    
    if not variants:
        print("Не удалось загрузить варианты из config/core.yaml")
        sys.exit(1)
    
    print("\n" + "="*60)
    print(" "*15 + "Калькулятор большой конечной арифметики")
    print("="*60 + "\n")
    print("Доступные варианты:\n")
    
    for i, var in enumerate(variants, 1):
        symbols_str = ', '.join(var['symbols'][:6])
        if len(var['symbols']) > 6:
            symbols_str += ', ...'
        print(f"  [{i}] {var['name']:12} | Z{var['size']:2} | Символы: {symbols_str}")
    
    print(f"\n  [0] Выход")
    print("\n" + "="*60)
    
    while True:
        try:
            choice = input("\nВыберите вариант (введите номер): ").strip()
            
            if choice == '0':
                print("\nДо свидания! 👋\n")
                sys.exit(0)
            
            idx = int(choice) - 1
            if 0 <= idx < len(variants):
                return variants[idx]['name']
            else:
                print(f"Ошибка: введите число от 0 до {len(variants)}")
                
        except ValueError:
            print("Ошибка: введите корректное число")
        except KeyboardInterrupt:
            print("\n\nПрервано.\n")
            sys.exit(0)


def main():
    variant_name = show_menu()
    calc = Calculator(variant_name)
    calc.run()


if __name__ == "__main__":
    main()