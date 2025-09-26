class Char:
    def __init__(self, value: str, field):
        if len(value) != 1:
            raise ValueError("Value must be a single character.")
        self.value = value
        self.field = field
    
    def __str__(self):
        return self.value

    def __repr__(self):
        return f"Char({self.value!r})"
    
    def __add__(self, other):
        return Char(self.field.add(self.value, other.value), self.field)

    def __sub__(self, other):
        return Char(self.field.subtract(self.value, other.value), self.field)

    def __mul__(self, other):
        return Char(self.field.multiply(self.value, other.value), self.field)

    def __truediv__(self, other):
        return Char(self.field.divide(self.value, other.value), self.field)
