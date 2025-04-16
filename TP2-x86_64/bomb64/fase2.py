# Buscar todos los pares (A, B) tal que:
# 1. A + B = 2079 porque x = 0x7FF = 2047 = 0b11111111111 (11 bits en 1) y A + B = x + 32 = 2047 + 32 = 2079
# 2. A ^ B < 0 (es negativo)

total = 2079
min_a = -3000
max_a = 3000

resultados = []

for a in range(min_a, max_a + 1):
    b = total - a
    xor = a ^ b
    if xor < 0:
        resultados.append((a, b, xor))

# Mostramos los resultados
for a, b, xor in resultados:
    print(f"A = {a}, B = {b}, A + B = {a + b}, A ^ B = {xor}")
