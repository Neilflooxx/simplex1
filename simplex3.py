import numpy as np
import os

def limpiar_pantalla():
    os.system('cls' if os.name == 'nt' else 'clear')

def ingresar_funcion_objetivo(num_vars):
    print(f"\nIngrese los coeficientes de la función objetivo ({num_vars} variables):")
    coeficientes = []
    for i in range(1, num_vars + 1):
        while True:
            try:
                val = float(input(f"Coeficiente de x{i}: "))
                coeficientes.append(val)
                break
            except ValueError:
                print("Valor inválido. Ingrese un número.")
    return coeficientes

def ingresar_restricciones(num_vars):
    restricciones = []
    rhs_list = []

    while True:
        print("\n¿Agregar una nueva restricción? (s/n)")
        respuesta = input().strip().lower()
        if respuesta != 's':
            break

        print("Ingrese los coeficientes de la restricción:")
        coef_restriccion = []
        for i in range(1, num_vars + 1):
            while True:
                try:
                    val = float(input(f"Coeficiente de x{i}: "))
                    coef_restriccion.append(val)
                    break
                except ValueError:
                    print("Valor inválido. Ingrese un número.")

        tipo = "<="  # simplificamos permitiendo solo restricciones ≤

        while True:
            try:
                rhs = float(input("Lado derecho (RHS): "))
                break
            except ValueError:
                print("Valor inválido. Ingrese un número.")

        restricciones.append(coef_restriccion)
        rhs_list.append(rhs)

    return restricciones, rhs_list

def metodo_simplex(c, A, b, tipo):
    num_vars = len(c)
    num_restricciones = len(b)

    tabla = np.zeros((num_restricciones + 1, num_vars + num_restricciones + 1))

    if tipo == 'max':
        tabla[-1, :num_vars] = c
    else:
        tabla[-1, :num_vars] = [-coef for coef in c]

    tabla[:-1, :num_vars] = A
    tabla[:-1, num_vars:num_vars + num_restricciones] = np.eye(num_restricciones)
    tabla[:-1, -1] = b

    var_basicas = [f's{i+1}' for i in range(num_restricciones)]
    var_no_basicas = [f'x{i+1}' for i in range(num_vars)]

    iteracion = 0
    while True:
        iteracion += 1
        print(f"\n{'-'*30} Iteración {iteracion} {'-'*30}")
        print(np.round(tabla, 2))

        col_pivote = np.argmin(tabla[-1, :-1])
        if tabla[-1, col_pivote] >= 0:
            break

        cocientes = []
        for i in range(num_restricciones):
            if tabla[i, col_pivote] > 0:
                cocientes.append((tabla[i, -1] / tabla[i, col_pivote], i))
            else:
                cocientes.append((np.inf, i))

        fila_pivote = min(cocientes, key=lambda x: x[0])[1]

        var_basicas[fila_pivote], var_no_basicas[col_pivote] = var_no_basicas[col_pivote], var_basicas[fila_pivote]

        tabla[fila_pivote, :] /= tabla[fila_pivote, col_pivote]

        for i in range(tabla.shape[0]):
            if i != fila_pivote:
                factor = tabla[i, col_pivote]
                tabla[i, :] -= factor * tabla[fila_pivote, :]

    solucion = {}
    for i in range(num_restricciones):
        col_base = np.where(np.isclose(tabla[i, :num_vars], 1))[0]
        if len(col_base) == 1:
            solucion[var_basicas[i]] = tabla[i, -1]

    for var in var_no_basicas:
        if var not in solucion:
            solucion[var] = 0

    z_optimo = tabla[-1, -1]
    if tipo == 'min':
        z_optimo = -z_optimo

    return solucion, z_optimo

# ========== PROGRAMA PRINCIPAL ==========

while True:
    limpiar_pantalla()
    print("=== MÉTODO SIMPLEX INTERACTIVO ===")

    try:
        num_vars = int(input("¿Cuántas variables tiene la función objetivo? "))
        if num_vars <= 0:
            print("El número de variables debe ser positivo.")
            continue
    except ValueError:
        print("Ingrese un número válido.")
        continue

    c = ingresar_funcion_objetivo(num_vars)
    A, b = ingresar_restricciones(num_vars)

    if len(A) == 0:
        print("Debe ingresar al menos una restricción.")
        continue

    while True:
        print("\n¿Desea maximizar o minimizar la función objetivo?")
        print("1. Maximizar")
        print("2. Minimizar")
        eleccion = input("Seleccione una opción (1/2): ")
        if eleccion in ['1', '2']:
            tipo = 'max' if eleccion == '1' else 'min'
            break
        else:
            print("Opción inválida.")

    print("\nResolviendo el problema...")

    solucion, z = metodo_simplex(c, A, b, tipo)

    print("\n🟩 SOLUCIÓN ÓPTIMA 🟩\n")
    for var in sorted(solucion.keys()):
        print(f"{var} = {round(solucion[var], 4)}")
    print(f"\n🔷 VALOR ÓPTIMO DE Z = {round(z, 4)} 🔷")

    input("\nPresiona Enter para una nueva operación...")
