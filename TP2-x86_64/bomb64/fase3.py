def binary_search_with_count(arr, target):
    left = 0
    right = len(arr) - 1
    comparisons = 0

    while left <= right:
        mid = (left + right) // 2
        comparisons += 1

        if arr[mid] == target:
            return comparisons, mid
        elif target < arr[mid]:
            right = mid - 1
        else:
            left = mid + 1

    return -1, -1  # Not found

# Cargar palabras desde archivo
with open("TP2-x86_64/bomb64/palabras.txt", "r", encoding="utf-8") as f:
    wordlist = [line.strip() for line in f if line.strip()]

# Buscar palabra que cumpla con la condición
for palabra in wordlist:
    comparaciones, indice = binary_search_with_count(wordlist, palabra)
    if 7 <= comparaciones <= 11:
        print(f"¡Input válido encontrado!: {palabra} {comparaciones}")
        print(f"Índice en el arreglo: {indice}")
        break
else:
    print("No se encontró ninguna palabra que cumpla con la condición.")
